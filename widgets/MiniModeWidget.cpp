#include "MiniModeWidget.h"
#include <QHBoxLayout>
#include <QScrollArea>
#include <QDate>
#include <QTime>
#include <QEvent>
#include <QGraphicsDropShadowEffect>
#include "StyleHelper.h"
#include "UiConstants.h"
#include "../models/DatabaseManager.h"
#include "ScheduleEdit.h"

MiniModeWidget::MiniModeWidget(QWidget *parent) : QWidget(parent) {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    m_contentWidget = new QWidget(this);
    m_contentWidget->setObjectName("miniContent");
    mainLayout->addWidget(m_contentWidget);

    auto* contentLayout = new QVBoxLayout(m_contentWidget);
    contentLayout->setContentsMargins(20, 20, 20, 20);
    contentLayout->setSpacing(15);

    // Header: Date & Time
    auto* headerLayout = new QHBoxLayout();
    m_dateLabel = new QLabel(this);
    m_dateLabel->setStyleSheet(QString("font-weight: bold; font-size: 15px; color: %1;").arg(StyleHelper::getTextColor()));
    headerLayout->addWidget(m_dateLabel);
    headerLayout->addStretch();

    QPushButton* pinBtn = new QPushButton("📌", this);
    pinBtn->setObjectName("miniPinBtn");
    pinBtn->setFixedSize(28, 28);
    pinBtn->setCheckable(true);
    pinBtn->setCursor(Qt::PointingHandCursor);
    connect(pinBtn, &QPushButton::toggled, this, &MiniModeWidget::pinRequested);
    headerLayout->addWidget(pinBtn);

    QPushButton* settingsBtn = new QPushButton(this);
    settingsBtn->setObjectName("miniSettingsBtn");
    settingsBtn->setFixedSize(28, 28);
    settingsBtn->setCursor(Qt::PointingHandCursor);
    connect(settingsBtn, &QPushButton::clicked, this, &MiniModeWidget::settingsRequested);
    headerLayout->addWidget(settingsBtn);

    contentLayout->addLayout(headerLayout);

    m_timeLabel = new QLabel(this);
    m_timeLabel->setStyleSheet(QString("font-size: 32px; font-weight: bold; color: %1;").arg(StyleHelper::getPrimaryColor()));
    contentLayout->addWidget(m_timeLabel);

    // Schedule List
    auto* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setStyleSheet("background: transparent; border: none;");

    auto* scrollContent = new QWidget(scroll);
    scrollContent->setStyleSheet("background: transparent;");
    m_scheduleLayout = new QVBoxLayout(scrollContent);
    m_scheduleLayout->setContentsMargins(0, 5, 5, 5);
    m_scheduleLayout->setSpacing(10);
    m_scheduleLayout->addStretch();

    scroll->setWidget(scrollContent);
    scroll->setStyleSheet(StyleHelper::getScrollbarStyle());
    contentLayout->addWidget(scroll, 1);

    // Footer: Back Button
    QPushButton* backBtn = new QPushButton("달력 보기", this);
    backBtn->setObjectName("miniBackBtn");
    backBtn->setFixedHeight(40);
    backBtn->setCursor(Qt::PointingHandCursor);
    connect(backBtn, &QPushButton::clicked, this, &MiniModeWidget::backToNormal);
    contentLayout->addWidget(backBtn);

    m_realTimeTimer = new QTimer(this);
    connect(m_realTimeTimer, &QTimer::timeout, this, &MiniModeWidget::updateDateTime);
    m_realTimeTimer->start(1000);

    updateDateTime();
    updateStyle();
}

void MiniModeWidget::updateDateTime() {
    QDate today = QDate::currentDate();
    m_dateLabel->setText(today.toString("yyyy년 MM월 dd일 dddd"));
    m_timeLabel->setText(QTime::currentTime().toString("HH:mm:ss"));
}

void MiniModeWidget::refresh() {
    m_itemDataMap.clear();
    QLayoutItem *item;
    while ((item = m_scheduleLayout->takeAt(0)) != nullptr) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }

    auto schedules = DatabaseManager::instance().getSchedulesForDay(QDate::currentDate());
    if (schedules.isEmpty()) {
        QLabel* empty = new QLabel("오늘 일정이 없습니다.", this);
        empty->setStyleSheet("color: #999; font-style: italic; padding: 20px; background: transparent;");
        empty->setAlignment(Qt::AlignCenter);
        m_scheduleLayout->addWidget(empty);
    } else {
        for (const auto& s : schedules) {
            QWidget* itemWidget = new QWidget(this);
            itemWidget->setObjectName("itemWidget");
            itemWidget->setCursor(Qt::PointingHandCursor);
            itemWidget->setAttribute(Qt::WA_Hover);
            itemWidget->installEventFilter(this);

            QString colorStr = s.color.isEmpty() ? StyleHelper::getPrimaryColor() : s.color;
            itemWidget->setStyleSheet(StyleHelper::getItemBaseStyle(colorStr));

            QHBoxLayout* itemLayout = new QHBoxLayout(itemWidget);
            itemLayout->setContentsMargins(15, 12, 15, 12);
            itemLayout->setSpacing(10);

            QLabel* titleLabel = new QLabel(s.title, itemWidget);
            titleLabel->setStyleSheet(QString("font-weight: bold; font-size: 13px; color: %1; border: none; background: transparent;").arg(StyleHelper::getTextColor()));
            itemLayout->addWidget(titleLabel, 1);

            QString startStr = s.start.toString("HH:mm");
            QString endStr   = s.end.toString("HH:mm");
            bool isAllDay    = (startStr == "00:00" && endStr == "00:00");

            if (!isAllDay) {
                QLabel* timeLabel = new QLabel(QString("%1~%2").arg(startStr, endStr), itemWidget);
                timeLabel->setStyleSheet("color: #888; font-size: 11px; border: none; background: transparent;");
                timeLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
                itemLayout->addWidget(timeLabel);
            } else {
                itemLayout->addStretch(0);
            }

            m_itemDataMap[itemWidget] = s;
            m_scheduleLayout->addWidget(itemWidget);
        }
    }
    m_scheduleLayout->addStretch();
}

void MiniModeWidget::updateStyle() {
    m_contentWidget->setStyleSheet(StyleHelper::getMiniModeContentStyle());
    m_timeLabel->setStyleSheet(StyleHelper::getMiniTimeStyle());
    m_dateLabel->setStyleSheet(StyleHelper::getMiniDateStyle());

    if (QPushButton* pBtn = findChild<QPushButton*>("miniPinBtn")) pBtn->setStyleSheet(StyleHelper::getPinButtonStyle());
    if (QPushButton* sBtn = findChild<QPushButton*>("miniSettingsBtn")) sBtn->setStyleSheet(StyleHelper::getBtnModifyStyle());
    if (QPushButton* bBtn = findChild<QPushButton*>("miniBackBtn")) bBtn->setStyleSheet(StyleHelper::getBtnSaveStyle());

    refresh();
}

void MiniModeWidget::setPinned(bool pinned) {
    if (QPushButton* pBtn = findChild<QPushButton*>("miniPinBtn")) {
        pBtn->setChecked(pinned);
    }
}

bool MiniModeWidget::eventFilter(QObject *obj, QEvent *event) {
    QWidget *widget = qobject_cast<QWidget*>(obj);
    if (!widget || !m_itemDataMap.contains(widget)) return QWidget::eventFilter(obj, event);

    QString colorStr = m_itemDataMap[widget].color;
    if (colorStr.isEmpty()) colorStr = StyleHelper::getPrimaryColor();

    if (event->type() == QEvent::MouseButtonPress) {
        ScheduleEditDialog *editDialog = new ScheduleEditDialog(m_itemDataMap[widget]);
        editDialog->setAttribute(Qt::WA_DeleteOnClose);
        editDialog->setWindowModality(Qt::ApplicationModal);
        connect(editDialog, &ScheduleEditDialog::scheduleSaved, this, &MiniModeWidget::scheduleChanged);
        connect(editDialog, &ScheduleEditDialog::scheduleDeleted, this, &MiniModeWidget::scheduleChanged);
        editDialog->show();
        return true;
    } 
    else if (event->type() == QEvent::HoverEnter) {
        widget->setStyleSheet(StyleHelper::getItemHoverStyle(colorStr));
        QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(widget);
        shadow->setBlurRadius(15); shadow->setYOffset(4); shadow->setColor(QColor(0, 0, 0, 40));
        widget->setGraphicsEffect(shadow);
    } 
    else if (event->type() == QEvent::HoverLeave) {
        widget->setStyleSheet(StyleHelper::getItemBaseStyle(colorStr));
        widget->setGraphicsEffect(nullptr);
    }

    return QWidget::eventFilter(obj, event);
}
