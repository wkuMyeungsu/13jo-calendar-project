#include "DayCell.h"
#include <QVBoxLayout>
#include <QMouseEvent>

DayCell::DayCell(QWidget* parent) : QFrame(parent) {
    setFrameStyle(QFrame::Box | QFrame::Plain);
    setFixedSize(120, 120); // 크기를 조금 늘림

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(2, 2, 2, 2);
    mainLayout->setSpacing(2);

    // 날짜 표시 라벨
    m_dateLabel = new QLabel(this);
    m_dateLabel->setStyleSheet("font-weight: bold; color: #333;");
    mainLayout->addWidget(m_dateLabel, 0, Qt::AlignTop | Qt::AlignLeft);

    // 일정 표시용 레이아웃
    m_scheduleLayout = new QVBoxLayout();
    m_scheduleLayout->setSpacing(1);
    mainLayout->addLayout(m_scheduleLayout);

    mainLayout->addStretch();

    // (+) 힌트 라벨 설정
    m_plusHint = new QLabel("+", this);
    m_plusHint->setAlignment(Qt::AlignCenter);
    m_plusHint->setStyleSheet("font-size: 20px; color: #4A90E2;");
    m_plusHint->setAttribute(Qt::WA_TransparentForMouseEvents);

    m_plusOpacity = new QGraphicsOpacityEffect(m_plusHint);
    m_plusOpacity->setOpacity(0.0);
    m_plusHint->setGraphicsEffect(m_plusOpacity);

    m_plusAnim = new QPropertyAnimation(m_plusOpacity, "opacity", this);

    mainLayout->addWidget(m_plusHint, 0, Qt::AlignBottom | Qt::AlignRight);
}

void DayCell::setDate(const QDate& date) {
    m_date = date;
    m_dateLabel->setText(QString::number(date.day()));

    // 토요일/일요일 색상 변경
    if (date.dayOfWeek() == 6) // 토요일
        m_dateLabel->setStyleSheet("font-weight: bold; color: blue;");
    else if (date.dayOfWeek() == 7) // 일요일
        m_dateLabel->setStyleSheet("font-weight: bold; color: red;");
}

void DayCell::setSchedules(const QList<QVariantMap>& schedules) {
    // 기존 일정 라벨 제거
    QLayoutItem *item;
    while ((item = m_scheduleLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }

    // 최대 3개까지만 표시
    int count = 0;
    for (const auto& schedule : schedules) {
        if (count >= 3) {
            QLabel* moreLabel = new QLabel(QString("+ %1 more").arg(schedules.size() - count), this);
            moreLabel->setStyleSheet("font-size: 9px; color: gray;");
            m_scheduleLayout->addWidget(moreLabel);
            break;
        }
        QLabel* label = new QLabel(schedule["title"].toString(), this);
        QString color = schedule["color"].toString();
        label->setStyleSheet(QString("background-color: %1; color: white; border-radius: 2px; padding: 1px; font-size: 10px;").arg(color));
        label->setToolTip(schedule["title"].toString());
        m_scheduleLayout->addWidget(label);
        count++;
    }
}

void DayCell::enterEvent(QEnterEvent* e) {
    m_plusAnim->stop();
    m_plusAnim->setDuration(150);
    m_plusAnim->setStartValue(m_plusOpacity->opacity());
    m_plusAnim->setEndValue(0.45);
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
