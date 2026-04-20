#include "DayCell.h"
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QGraphicsOpacityEffect>
#include <QDateTime>

DayCell::DayCell(QWidget* parent) 
    : QFrame(parent)
    , m_wasTimeShown(false)
    , m_lastMaxSchedules(0)
{
    setFrameStyle(QFrame::NoFrame);
    setMinimumSize(50, 50);
    setStyleSheet(
        "DayCell { border: 0.5px solid #E0E0E0; background-color: white; }"
        "DayCell:hover { background-color: #F5F8FF; }"
    );

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(2, 2, 2, 2);
    mainLayout->setSpacing(2);

    m_dateLabel = new QLabel(this);
    m_dateLabel->setStyleSheet("font-weight: bold; color: #333; border: none;");
    mainLayout->addWidget(m_dateLabel, 0, Qt::AlignTop | Qt::AlignLeft);

    m_scheduleLayout = new QVBoxLayout();
    m_scheduleLayout->setSpacing(1);
    mainLayout->addLayout(m_scheduleLayout);

    mainLayout->addStretch();

    m_plusButton = new QPushButton("+", this);
    m_plusButton->setFixedSize(24, 24);
    m_plusButton->setCursor(Qt::PointingHandCursor);
    m_plusButton->setStyleSheet(
        "QPushButton {"
        "  border: none;"
        "  background-color: #4A90E2;"
        "  color: white;"
        "  border-radius: 12px;"
        "  font-size: 16px;"
        "  font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "  background-color: #357ABD;"
        "}"
    );

    m_plusOpacity = new QGraphicsOpacityEffect(m_plusButton);
    m_plusOpacity->setOpacity(0.0);
    m_plusButton->setGraphicsEffect(m_plusOpacity);

    m_plusAnim = new QPropertyAnimation(m_plusOpacity, "opacity", this);

    mainLayout->addWidget(m_plusButton, 0, Qt::AlignBottom | Qt::AlignRight);

    connect(m_plusButton, &QPushButton::clicked, [this]() {
        emit addRequested(m_date);
    });
}

void DayCell::setDate(const QDate& date) {
    m_date = date;
    m_dateLabel->setText(QString::number(date.day()));

    if (date == QDate::currentDate()) {
        m_dateLabel->setFixedSize(26, 26);
        m_dateLabel->setAlignment(Qt::AlignCenter);
        m_dateLabel->setStyleSheet(
            "background-color: #4A90E2; color: white; border-radius: 13px; "
            "font-weight: bold; border: none;"
        );
    } else {
        m_dateLabel->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
        m_dateLabel->setMinimumSize(0, 0);
        m_dateLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
        if (date.dayOfWeek() == 6)
            m_dateLabel->setStyleSheet("font-weight: bold; color: #1565C0; border: none;");
        else if (date.dayOfWeek() == 7)
            m_dateLabel->setStyleSheet("font-weight: bold; color: #D32F2F; border: none;");
        else
            m_dateLabel->setStyleSheet("font-weight: bold; color: #333; border: none;");
    }
}

void DayCell::setSchedules(const QList<QVariantMap>& schedules) {
    m_currentSchedules = schedules;
    
    // 1. 가용 높이 계산 (날짜 라벨과 여백 제외)
    int dateLabelHeight = m_dateLabel->height() > 0 ? m_dateLabel->height() : 20;
    int availableHeight = this->height() - dateLabelHeight - 15; // 마진 고려
    
    // 2. 최대 표시 가능 개수 계산 (일정 라벨 하나당 약 18px 기준)
    int maxSchedules = qMax(0, availableHeight / 18);
    m_lastMaxSchedules = maxSchedules;
    
    bool showTime = this->width() > 110;
    m_wasTimeShown = showTime;

    this->setUpdatesEnabled(false);
    
    QLayoutItem *item;
    while ((item = m_scheduleLayout->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }

    int count = 0;
    bool overflow = schedules.size() > maxSchedules;
    int limit = overflow ? maxSchedules - 1 : maxSchedules;

    for (const auto& schedule : schedules) {
        if (count >= limit && overflow) {
            QLabel* moreLabel = new QLabel(QString("+ %1 more").arg(schedules.size() - count), this);
            moreLabel->setStyleSheet("font-size: 9px; color: gray; border: none;");
            m_scheduleLayout->addWidget(moreLabel);
            break;
        }
        
        if (count >= maxSchedules) break;

        QDateTime start = QDateTime::fromString(schedule["start"].toString(), "yyyy-MM-dd HH:mm:ss");
        QDateTime end = QDateTime::fromString(schedule["end"].toString(), "yyyy-MM-dd HH:mm:ss");

        bool isStartDay = (start.date() == m_date);
        bool isEndDay = (end.date() == m_date);
        bool isAllDay = (start.time() <= QTime(0, 0, 5) && end.time() >= QTime(23, 59, 0)) 
                        || (start.date() != end.date());

        QLabel* label = new QLabel(this);
        QString displayText = "";

        if (isStartDay || m_date.dayOfWeek() == 7 || m_date.day() == 1) {
            displayText = schedule["title"].toString();
            if (showTime && isStartDay && !isAllDay) {
                displayText = start.toString("HH:mm ") + displayText;
            }
        }
        label->setText(displayText);

        QString color = schedule["color"].toString();
        QString style;

        if (isAllDay) {
            QString leftRadius = isStartDay ? "4px" : "0px";
            QString rightRadius = isEndDay ? "4px" : "0px";
            style = QString("background-color: %1; color: white; border: none; "
                            "border-top-left-radius: %2; border-bottom-left-radius: %2; "
                            "border-top-right-radius: %3; border-bottom-right-radius: %3; "
                            "margin: 0px; padding: 1px; font-size: 10px; font-weight: bold;")
                    .arg(color).arg(leftRadius).arg(rightRadius);
        } else {
            style = QString("background-color: transparent; color: %1; border: none; "
                            "font-weight: bold; font-size: 10px; padding: 1px;")
                    .arg(color);
            if (!displayText.isEmpty() && !displayText.startsWith("•")) {
                label->setText("• " + label->text());
            }
        }

        label->setStyleSheet(style);
        label->setToolTip(QString("%1\n%2 ~ %3").arg(schedule["title"].toString())
                          .arg(start.toString("MM/dd HH:mm")).arg(end.toString("MM/dd HH:mm")));
        m_scheduleLayout->addWidget(label);
        count++;
    }

    this->setUpdatesEnabled(true);
    this->update();
}

void DayCell::resizeEvent(QResizeEvent* e) {
    QFrame::resizeEvent(e);
    
    // 가용 개수 다시 계산
    int dateLabelHeight = m_dateLabel->height() > 0 ? m_dateLabel->height() : 20;
    int availableHeight = this->height() - dateLabelHeight - 15;
    int currentMax = qMax(0, availableHeight / 18);
    
    bool currentTimeShown = (this->width() > 110);
    
    // 높이가 변해 표시 가능한 개수가 바뀌었거나, 너비가 변해 시간 표시 상태가 바뀐 경우에만 리드로우
    if (currentMax != m_lastMaxSchedules || currentTimeShown != m_wasTimeShown) {
        if (!m_currentSchedules.isEmpty()) {
            setSchedules(m_currentSchedules);
        } else {
            m_lastMaxSchedules = currentMax;
            m_wasTimeShown = currentTimeShown;
        }
    }
}

void DayCell::enterEvent(QEnterEvent* e) {
    m_plusAnim->stop();
    m_plusAnim->setDuration(150);
    m_plusAnim->setStartValue(m_plusOpacity->opacity());
    m_plusAnim->setEndValue(1.0);
    m_plusAnim->start();
    QFrame::enterEvent(e);
}

void DayCell::leaveEvent(QEvent* e) {
    m_plusAnim->stop();
    m_plusAnim->setDuration(150);
    m_plusAnim->setStartValue(m_plusOpacity->opacity());
    m_plusAnim->setEndValue(0.0);
    m_plusAnim->start();
    QFrame::leaveEvent(e);
}

void DayCell::mouseDoubleClickEvent(QMouseEvent* e) {
    if (e->button() == Qt::LeftButton) {
        emit dayDoubleClicked(m_date);
    }
}
