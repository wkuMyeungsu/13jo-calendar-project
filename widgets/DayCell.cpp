#include "DayCell.h"
#include <QVBoxLayout>
#include <QMouseEvent>

DayCell::DayCell(QWidget* parent) : QFrame(parent) {
    setFrameStyle(QFrame::Box | QFrame::Plain);
    setFixedSize(100, 100); // 설계서 높이 기준 적절히 조절

    QVBoxLayout* layout = new QVBoxLayout(this);

    // 날짜 표시 라벨
    m_dateLabel = new QLabel(this);
    layout->addWidget(m_dateLabel, 0, Qt::AlignTop | Qt::AlignLeft);

    // (+) 힌트 라벨 설정
    m_plusHint = new QLabel("+", this);
    m_plusHint->setAlignment(Qt::AlignCenter);
    m_plusHint->setStyleSheet("font-size: 24px; color: #4A90E2;");
    m_plusHint->setAttribute(Qt::WA_TransparentForMouseEvents); // 마우스 이벤트 통과

    // 페이드 효과를 위한 GraphicsEffect
    m_plusOpacity = new QGraphicsOpacityEffect(m_plusHint);
    m_plusOpacity->setOpacity(0.0);
    m_plusHint->setGraphicsEffect(m_plusOpacity);

    // 애니메이션 설정
    m_plusAnim = new QPropertyAnimation(m_plusOpacity, "opacity", this);

    layout->addWidget(m_plusHint, 1, Qt::AlignCenter);
}

void DayCell::setDate(const QDate& date) {
    m_date = date;
    m_dateLabel->setText(QString::number(date.day()));
}

void DayCell::enterEvent(QEnterEvent* e) {
    m_plusAnim->stop();
    m_plusAnim->setDuration(150);
    m_plusAnim->setStartValue(m_plusOpacity->opacity());
    m_plusAnim->setEndValue(0.45); // 살짝 투명하게
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
        // 여기서 나중에 다이얼로그 호출 시그널 발생
    }
}