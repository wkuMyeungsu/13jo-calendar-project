#include "DayCell.h"
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QGraphicsOpacityEffect>

DayCell::DayCell(QWidget* parent) : QFrame(parent) {
    // 테두리 스타일을 CSS로 제어하기 위해 기본 프레임 설정 변경
    setFrameStyle(QFrame::NoFrame);
    
    // 기본 최소 크기만 지정 (고정 크기 제거)
    setMinimumSize(50, 50);
    
    // 외곽선이 겹치지 않도록 스타일 시트 적용
    setStyleSheet("DayCell { border: 0.5px solid #E0E0E0; background-color: white; }");

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

    int count = 0;
    for (const auto& schedule : schedules) {
        if (count >= 3) {
            QLabel* moreLabel = new QLabel(QString("+ %1 more").arg(schedules.size() - count), this);
            moreLabel->setStyleSheet("font-size: 9px; color: gray;");
            m_scheduleLayout->addWidget(moreLabel);
            break;
        }

        QDateTime start = QDateTime::fromString(schedule["start"].toString(), "yyyy-MM-dd HH:mm:ss");
        QDateTime end = QDateTime::fromString(schedule["end"].toString(), "yyyy-MM-dd HH:mm:ss");

        bool isStartDay = (start.date() == m_date);
        bool isEndDay = (end.date() == m_date);
        
        // 하루종일 여부 판단 (시간이 00:00~23:59 이거나 기간이 1일 이상인 경우)
        bool isAllDay = (start.time() <= QTime(0, 0, 5) && end.time() >= QTime(23, 59, 0)) 
                        || (start.date() != end.date());

        QLabel* label = new QLabel(this);
        
        // 텍스트 설정: 시작일이거나 매주 첫날(일요일)일 때 제목 표시
        if (isStartDay || m_date.dayOfWeek() == 7 || m_date.day() == 1) {
            label->setText(schedule["title"].toString());
        }

        QString color = schedule["color"].toString();
        QString style;

        if (isAllDay) {
            // 연속된 바 스타일
            QString leftRadius = isStartDay ? "4px" : "0px";
            QString rightRadius = isEndDay ? "4px" : "0px";
            QString marginLeft = isStartDay ? "2px" : "0px";
            QString marginRight = isEndDay ? "2px" : "0px";

            style = QString("background-color: %1; color: white; "
                            "border-top-left-radius: %2; border-bottom-left-radius: %2; "
                            "border-top-right-radius: %3; border-bottom-right-radius: %3; "
                            "margin-left: %4; margin-right: %5; "
                            "padding: 1px; font-size: 10px; font-weight: bold;")
                    .arg(color).arg(leftRadius).arg(rightRadius).arg(marginLeft).arg(marginRight);
        } else {
            // 배경색 없는 강조 스타일
            style = QString("background-color: transparent; color: %1; "
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
