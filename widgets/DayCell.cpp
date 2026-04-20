#include "DayCell.h"
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QGraphicsOpacityEffect>

DayCell::DayCell(QWidget* parent) : QFrame(parent) {
    setFrameStyle(QFrame::NoFrame);
    setMinimumSize(50, 50);
    setStyleSheet("DayCell { border: 0.5px solid #E0E0E0; background-color: white; }");

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

    // (+) 버튼 설정
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

    // 버튼 클릭 시 시그널 발생
    connect(m_plusButton, &QPushButton::clicked, [this]() {
        emit addRequested(m_date);
    });
}

void DayCell::setDate(const QDate& date) {
    m_date = date;
    m_dateLabel->setText(QString::number(date.day()));
    if (date.dayOfWeek() == 6)
        m_dateLabel->setStyleSheet("font-weight: bold; color: blue; border: none;");
    else if (date.dayOfWeek() == 7)
        m_dateLabel->setStyleSheet("font-weight: bold; color: red; border: none;");
}

void DayCell::setSchedules(const QList<QVariantMap>& schedules) {
    QLayoutItem *item;
    while ((item = m_scheduleLayout->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }

    int count = 0;
    for (const auto& schedule : schedules) {
        if (count >= 3) {
            QLabel* moreLabel = new QLabel(QString("+ %1 more").arg(schedules.size() - count), this);
            moreLabel->setStyleSheet("font-size: 9px; color: gray; border: none;");
            m_scheduleLayout->addWidget(moreLabel);
            break;
        }

        QDateTime start = QDateTime::fromString(schedule["start"].toString(), "yyyy-MM-dd HH:mm:ss");
        QDateTime end = QDateTime::fromString(schedule["end"].toString(), "yyyy-MM-dd HH:mm:ss");

        bool isStartDay = (start.date() == m_date);
        bool isEndDay = (end.date() == m_date);
        bool isAllDay = (start.time() <= QTime(0, 0, 5) && end.time() >= QTime(23, 59, 0)) 
                        || (start.date() != end.date());

        QLabel* label = new QLabel(this);
        if (isStartDay || m_date.dayOfWeek() == 7 || m_date.day() == 1) {
            label->setText(schedule["title"].toString());
        }

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
            label->setText("• " + label->text());
        }

        label->setStyleSheet(style);
        label->setToolTip(QString("%1\n%2 ~ %3").arg(schedule["title"].toString())
                          .arg(start.toString("MM/dd HH:mm")).arg(end.toString("MM/dd HH:mm")));
        m_scheduleLayout->addWidget(label);
        count++;
    }
}

void DayCell::enterEvent(QEnterEvent* e) {
    m_plusAnim->stop();
    m_plusAnim->setDuration(150);
    m_plusAnim->setStartValue(m_plusOpacity->opacity());
    m_plusAnim->setEndValue(1.0); // 완전히 보이게
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
