#include "ScheduleManagerWidget.h"
#include "ScheduleEdit.h"
#include "ScheduleModifyWidget.h"
#include "../models/DatabaseManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QLabel>
#include <QPushButton>

ScheduleManagerWidget::ScheduleManagerWidget(const QDate& date, QWidget *parent) : QWidget(parent), m_date(date) {
    setFixedSize(400, 500);
    setWindowTitle(date.toString("yyyy-MM-dd") + " 일정 관리");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 상단 헤더 (날짜 제목만 유지)
    m_titleLabel = new QLabel(date.toString("yyyy년 MM월 dd일 일정"), this);
    m_titleLabel->setStyleSheet("font-size: 16px; font-weight: bold; margin-bottom: 10px;");
    mainLayout->addWidget(m_titleLabel);

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    QWidget *scrollContent = new QWidget();
    m_listLayout = new QVBoxLayout(scrollContent);
    m_listLayout->setAlignment(Qt::AlignTop);
    scrollArea->setWidget(scrollContent);
    mainLayout->addWidget(scrollArea);

    QPushButton *addBtn = new QPushButton("+ 새 일정 추가", this);
    addBtn->setStyleSheet("background-color: #4CAF50; color: white; padding: 10px; font-weight: bold;");
    mainLayout->addWidget(addBtn);

    connect(addBtn, &QPushButton::clicked, this, &ScheduleManagerWidget::openAddWidget);

    refreshList();
}

void ScheduleManagerWidget::refreshList() {
    // 기존 아이템 제거
    QLayoutItem *item;
    while ((item = m_listLayout->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }

    QList<QVariantMap> schedules = DatabaseManager::instance().getSchedulesForDay(m_date);

    if (schedules.isEmpty()) {
        m_listLayout->addWidget(new QLabel("등록된 일정이 없습니다.", this));
    } else {
        for (const auto& s : schedules) {
            QWidget *itemWidget = new QWidget(this);
            itemWidget->setStyleSheet("background: #f9f9f9; border: 1px solid #ddd; border-radius: 5px; margin-bottom: 5px;");
            QHBoxLayout *itemLayout = new QHBoxLayout(itemWidget);

            QVBoxLayout *textLayout = new QVBoxLayout();
            QLabel *title = new QLabel(s["title"].toString(), this);
            title->setStyleSheet("font-weight: bold; border: none;");
            
            QLabel *time = new QLabel(QString("%1 ~ %2").arg(s["start"].toString().mid(11, 5)).arg(s["end"].toString().mid(11, 5)), this);
            time->setStyleSheet("color: #666; font-size: 11px; border: none;");

            textLayout->addWidget(title);
            textLayout->addWidget(time);
            itemLayout->addLayout(textLayout);

            QPushButton *editBtn = new QPushButton("수정", this);
            editBtn->setFixedWidth(60);
            itemLayout->addWidget(editBtn);

            connect(editBtn, &QPushButton::clicked, [this, s]() {
                openEditWidget(s);
            });

            m_listLayout->addWidget(itemWidget);
        }
    }
}

void ScheduleManagerWidget::openAddWidget() {
    ScheduleInputWidget *inputWidget = new ScheduleInputWidget(m_date);
    inputWidget->setAttribute(Qt::WA_DeleteOnClose);
    inputWidget->setWindowModality(Qt::ApplicationModal);
    
    connect(inputWidget, &ScheduleInputWidget::scheduleSaved, this, [this]() {
        refreshList();
        emit dataChanged();
    });
    
    inputWidget->show();
}

void ScheduleManagerWidget::openEditWidget(const QVariantMap& data) {
    ScheduleModifyWidget *modifyWidget = new ScheduleModifyWidget(data);
    modifyWidget->setAttribute(Qt::WA_DeleteOnClose);
    modifyWidget->setWindowModality(Qt::ApplicationModal);
    modifyWidget->setWindowTitle("일정 수정/삭제");

    connect(modifyWidget, &ScheduleModifyWidget::scheduleUpdated, this, [this]() {
        refreshList();
        emit dataChanged();
    });
    connect(modifyWidget, &ScheduleModifyWidget::scheduleDeleted, this, [this]() {
        refreshList();
        emit dataChanged();
    });

    modifyWidget->show();
}
