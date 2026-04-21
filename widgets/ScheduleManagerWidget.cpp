#include "ScheduleManagerWidget.h"
#include "ScheduleEdit.h"
#include "ScheduleModifyWidget.h"
#include "SettingsWidget.h"
#include "StyleHelper.h"
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
    setFixedSize(StyleHelper::WIDGET_WIDTH, StyleHelper::WIDGET_HEIGHT);
    setWindowTitle(date.toString("yyyy-MM-dd") + " 일정 관리");
    setStyleSheet(QString("background-color: %1;").arg(StyleHelper::getBgColor()));

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(StyleHelper::CONTENT_MARGIN, StyleHelper::CONTENT_MARGIN, StyleHelper::CONTENT_MARGIN, StyleHelper::CONTENT_MARGIN);
    mainLayout->setSpacing(10);

    m_titleLabel = new QLabel(date.toString("yyyy년 MM월 dd일 일정"), this);
    m_titleLabel->setStyleSheet(StyleHelper::getHeaderStyle());
    mainLayout->addWidget(m_titleLabel);

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet(StyleHelper::getScrollbarStyle());

    QWidget *scrollContent = new QWidget();
    scrollContent->setStyleSheet("background: transparent;");
    m_listLayout = new QVBoxLayout(scrollContent);
    m_listLayout->setContentsMargins(5, 5, 15, 5);
    m_listLayout->setSpacing(12);
    m_listLayout->setAlignment(Qt::AlignTop);
    
    scrollArea->setWidget(scrollContent);
    mainLayout->addWidget(scrollArea);

    QPushButton *addBtn = new QPushButton("+ 새 일정 추가", this);
    addBtn->setCursor(Qt::PointingHandCursor);
    addBtn->setStyleSheet(StyleHelper::getBtnSaveStyle());
    mainLayout->addWidget(addBtn);

    connect(addBtn, &QPushButton::clicked, this, &ScheduleManagerWidget::openAddWidget);

    refreshList();
}

void ScheduleManagerWidget::refreshList() {
    m_itemDataMap.clear();
    QLayoutItem *item;
    while ((item = m_listLayout->takeAt(0)) != nullptr) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }

    auto schedules = DatabaseManager::instance().getSchedulesForDay(m_date);
    if (schedules.isEmpty()) {
        QLabel *emptyLabel = new QLabel("등록된 일정이 없습니다.", this);
        emptyLabel->setAlignment(Qt::AlignCenter);
        emptyLabel->setStyleSheet(QString("color: #999; font-size: 14px; margin-top: 50px; background: transparent;"));
        m_listLayout->addWidget(emptyLabel);
        return;
    }

    for (const auto& s : schedules) {
        QWidget *itemWidget = new QWidget(this);
        itemWidget->setObjectName("itemWidget");
        itemWidget->setCursor(Qt::PointingHandCursor);
        itemWidget->setAttribute(Qt::WA_Hover);
        itemWidget->installEventFilter(this);
        
        QString colorStr = s["color"].toString();
        if (colorStr.isEmpty()) colorStr = StyleHelper::getPrimaryColor();
        itemWidget->setStyleSheet(StyleHelper::getItemBaseStyle(colorStr));

        QHBoxLayout *itemLayout = new QHBoxLayout(itemWidget);
        itemLayout->setContentsMargins(15, 12, 15, 12);

        QVBoxLayout *textLayout = new QVBoxLayout();
        QLabel *title = new QLabel(s["title"].toString(), itemWidget);
        title->setStyleSheet(QString("font-weight: bold; font-size: 14px; color: %1; border: none; background: transparent;").arg(StyleHelper::getTextColor()));
        
        QString timeStr = (s["all_day"].toInt() == 1) ? "하루 종일" : QString("%1 ~ %2").arg(s["start"].toString().mid(11, 5)).arg(s["end"].toString().mid(11, 5));
        QLabel *time = new QLabel(timeStr, itemWidget);
        time->setStyleSheet(QString("color: %1; font-size: 12px; border: none; background: transparent;").arg(StyleHelper::currentTheme == StyleHelper::Theme::Dark ? "#AAA" : "#777"));

        textLayout->addWidget(title);
        textLayout->addWidget(time);
        itemLayout->addLayout(textLayout, 2);

        QString content = s["content"].toString().simplified();
        if (!content.isEmpty()) {
            if (content.length() > 25) content = content.left(22) + "...";
            QLabel *noteLabel = new QLabel(content, itemWidget);
            noteLabel->setStyleSheet(QString("color: #999; font-size: 11px; font-style: italic; border: none; background: transparent; padding-left: 10px; padding-right: 5px;"));
            noteLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
            itemLayout->addWidget(noteLabel, 3);
        } else {
            itemLayout->addStretch(3);
        }

        m_itemDataMap[itemWidget] = s;
        m_listLayout->addWidget(itemWidget);
    }
}

bool ScheduleManagerWidget::eventFilter(QObject *obj, QEvent *event) {
    QWidget *widget = qobject_cast<QWidget*>(obj);
    if (!widget || !m_itemDataMap.contains(widget)) return QWidget::eventFilter(obj, event);

    QString colorStr = m_itemDataMap[widget]["color"].toString();
    if (colorStr.isEmpty()) colorStr = StyleHelper::getPrimaryColor();

    if (event->type() == QEvent::MouseButtonPress) {
        openEditWidget(m_itemDataMap[widget]);
        return true;
    } 
    else if (event->type() == QEvent::HoverEnter) {
        widget->setStyleSheet(StyleHelper::getItemHoverStyle(colorStr));
        QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(widget);
        shadow->setBlurRadius(15); shadow->setYOffset(4); shadow->setColor(QColor(0, 0, 0, (StyleHelper::currentTheme == StyleHelper::Theme::Dark ? 80 : 40)));
        widget->setGraphicsEffect(shadow);
    } 
    else if (event->type() == QEvent::HoverLeave) {
        widget->setStyleSheet(StyleHelper::getItemBaseStyle(colorStr));
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
