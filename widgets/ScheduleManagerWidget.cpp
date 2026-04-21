#include "ScheduleManagerWidget.h"
#include "ScheduleEdit.h"
#include "ScheduleModifyWidget.h"
#include "../models/DatabaseManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QLabel>
#include <QPushButton>
#include <QGraphicsDropShadowEffect>
#include <QEvent>
#include <QMouseEvent>

ScheduleManagerWidget::ScheduleManagerWidget(const QDate& date, QWidget *parent) : QWidget(parent), m_date(date) {
    setFixedSize(400, 500);
    setWindowTitle(date.toString("yyyy-MM-dd") + " 일정 관리");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(10);

    // 상단 헤더
    m_titleLabel = new QLabel(date.toString("yyyy년 MM월 dd일 일정"), this);
    m_titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #333; margin-bottom: 5px;");
    mainLayout->addWidget(m_titleLabel);

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 가로 스크롤 제거
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet(
        "QScrollArea { background: transparent; border: none; }"
        "QScrollBar:vertical { border: none; background: transparent; width: 8px; margin: 0px 0px 0px 0px; }"
        "QScrollBar::handle:vertical { background: #E0E0E0; min-height: 20px; border-radius: 4px; }"
        "QScrollBar::handle:vertical:hover { background: #BDBDBD; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { border: none; background: none; height: 0px; }"
        "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { background: none; }"
    );

    QWidget *scrollContent = new QWidget();
    scrollContent->setObjectName("scrollContent");
    scrollContent->setStyleSheet("#scrollContent { background: transparent; }");
    
    m_listLayout = new QVBoxLayout(scrollContent);
    m_listLayout->setContentsMargins(5, 5, 15, 5); // 우측 마진을 늘려 스크롤바 공간 확보
    m_listLayout->setSpacing(12);
    m_listLayout->setAlignment(Qt::AlignTop);
    
    scrollArea->setWidget(scrollContent);
    mainLayout->addWidget(scrollArea);

    QPushButton *addBtn = new QPushButton("+ 새 일정 추가", this);
    addBtn->setCursor(Qt::PointingHandCursor);
    addBtn->setStyleSheet(
        "QPushButton { background-color: #4CAF50; color: white; padding: 12px; font-size: 14px; font-weight: bold; border-radius: 6px; border: none; }"
        "QPushButton:hover { background-color: #45a049; }"
    );
    mainLayout->addWidget(addBtn);

    connect(addBtn, &QPushButton::clicked, this, &ScheduleManagerWidget::openAddWidget);

    refreshList();
}

void ScheduleManagerWidget::refreshList() {
    // 기존 아이템 제거 및 데이터 맵 초기화
    m_itemDataMap.clear();
    QLayoutItem *item;
    while ((item = m_listLayout->takeAt(0)) != nullptr) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }

    QList<QVariantMap> schedules = DatabaseManager::instance().getSchedulesForDay(m_date);

    if (schedules.isEmpty()) {
        QLabel *emptyLabel = new QLabel("등록된 일정이 없습니다.", this);
        emptyLabel->setAlignment(Qt::AlignCenter);
        emptyLabel->setStyleSheet("color: #999; font-size: 14px; margin-top: 50px;");
        m_listLayout->addWidget(emptyLabel);
    } else {
        for (const auto& s : schedules) {
            QWidget *itemWidget = new QWidget(this);
            itemWidget->setObjectName("itemWidget");
            itemWidget->setCursor(Qt::PointingHandCursor);
            itemWidget->setAttribute(Qt::WA_Hover);
            itemWidget->installEventFilter(this);
            
            // 일정 색상 가져오기 (기본값 #4A90E2)
            QString colorStr = s["color"].toString();
            if (colorStr.isEmpty()) colorStr = "#4A90E2";

            itemWidget->setStyleSheet(QString(
                "#itemWidget { background-color: white; border-left: 5px solid %1; border-radius: 6px; border-top: 1px solid #EEE; border-right: 1px solid #EEE; border-bottom: 1px solid #EEE; }"
            ).arg(colorStr));

            QHBoxLayout *itemLayout = new QHBoxLayout(itemWidget);
            itemLayout->setContentsMargins(15, 12, 15, 12);

            QVBoxLayout *textLayout = new QVBoxLayout();
            textLayout->setSpacing(4);

            QLabel *title = new QLabel(s["title"].toString(), itemWidget);
            title->setStyleSheet("font-weight: bold; font-size: 14px; color: #333; border: none; background: transparent;");
            
            QString timeStr;
            if (s["all_day"].toInt() == 1) {
                timeStr = "하루 종일";
            } else {
                timeStr = QString("%1 ~ %2").arg(s["start"].toString().mid(11, 5)).arg(s["end"].toString().mid(11, 5));
            }
            
            QLabel *time = new QLabel(timeStr, itemWidget);
            time->setStyleSheet("color: #777; font-size: 12px; border: none; background: transparent;");

            textLayout->addWidget(title);
            textLayout->addWidget(time);
            itemLayout->addLayout(textLayout, 2); // 좌측 영역 비중 2

            // 상세 노트 표시 영역 추가
            QString content = s["content"].toString().simplified();
            if (!content.isEmpty()) {
                // 글자수 제한 조정
                if (content.length() > 25) content = content.left(22) + "...";
                
                QLabel *noteLabel = new QLabel(content, itemWidget);
                // padding-right를 추가하여 스크롤바와 간격 유지
                noteLabel->setStyleSheet("color: #999; font-size: 11px; font-style: italic; border: none; background: transparent; padding-left: 10px; padding-right: 5px;");
                noteLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
                itemLayout->addWidget(noteLabel, 3); // 우측 영역 비중 3
            } else {
                itemLayout->addStretch(3); 
            }

            // 데이터 맵에 저장
            m_itemDataMap[itemWidget] = s;
            m_listLayout->addWidget(itemWidget);
        }
    }
}

bool ScheduleManagerWidget::eventFilter(QObject *obj, QEvent *event) {
    QWidget *widget = qobject_cast<QWidget*>(obj);
    if (!widget || !m_itemDataMap.contains(widget)) return QWidget::eventFilter(obj, event);

    if (event->type() == QEvent::MouseButtonPress) {
        openEditWidget(m_itemDataMap[widget]);
        return true;
    } 
    else if (event->type() == QEvent::HoverEnter) {
        widget->setStyleSheet(widget->styleSheet() + " #itemWidget { background-color: #F5F9FF; border-top: 1px solid #4A90E2; border-right: 1px solid #4A90E2; border-bottom: 1px solid #4A90E2; }");
        
        QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(widget);
        shadow->setBlurRadius(15);
        shadow->setXOffset(0);
        shadow->setYOffset(4);
        shadow->setColor(QColor(0, 0, 0, 40));
        widget->setGraphicsEffect(shadow);
    } 
    else if (event->type() == QEvent::HoverLeave) {
        // 스타일시트 복구 (아이템마다 border-left 색상이 다르므로 주의)
        QString colorStr = m_itemDataMap[widget]["color"].toString();
        if (colorStr.isEmpty()) colorStr = "#4A90E2";
        widget->setStyleSheet(QString(
            "#itemWidget { background-color: white; border-left: 5px solid %1; border-radius: 6px; border-top: 1px solid #EEE; border-right: 1px solid #EEE; border-bottom: 1px solid #EEE; }"
        ).arg(colorStr));
        widget->setGraphicsEffect(nullptr);
    }

    return QWidget::eventFilter(obj, event);
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
