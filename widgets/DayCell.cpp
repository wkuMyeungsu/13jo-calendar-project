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

    // 날짜 라벨 높이를 22px로 고정 → 모든 셀에서 바의 수직 시작 위치 통일
    m_dateLabel->setMinimumHeight(22);
    m_dateLabel->setMaximumHeight(22);

    if (date == QDate::currentDate()) {
        m_dateLabel->setFixedWidth(22);
        m_dateLabel->setAlignment(Qt::AlignCenter);
        m_dateLabel->setStyleSheet(
            "background-color: #4A90E2; color: white; border-radius: 11px; "
            "font-weight: bold; border: none;"
        );
    } else {
        m_dateLabel->setMinimumWidth(0);
        m_dateLabel->setMaximumWidth(QWIDGETSIZE_MAX);
        m_dateLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        if (date.dayOfWeek() == 6)
            m_dateLabel->setStyleSheet("font-weight: bold; color: #1565C0; border: none;");
        else if (date.dayOfWeek() == 7)
            m_dateLabel->setStyleSheet("font-weight: bold; color: #D32F2F; border: none;");
        else
            m_dateLabel->setStyleSheet("font-weight: bold; color: #333; border: none;");
    }
}

// 슬롯 하나를 렌더링. bar=true면 all-day 바 스타일, false면 시간 이벤트 스타일
static QLabel* makeScheduleLabel(const QVariantMap& schedule, const QDate& cellDate,
                                 bool showTime, QWidget* parent)
{
    QDateTime start = QDateTime::fromString(schedule["start"].toString(), "yyyy-MM-dd HH:mm:ss");
    QDateTime end   = QDateTime::fromString(schedule["end"].toString(),   "yyyy-MM-dd HH:mm:ss");
    bool isStartDay = (start.date() == cellDate);
    bool isEndDay   = (end.date()   == cellDate);
    bool isAllDay   = (start.time() <= QTime(0, 0, 5) && end.time() >= QTime(23, 59, 0))
                      || (start.date() != end.date());

    QString displayText;
    if (isStartDay || cellDate.dayOfWeek() == 7 || cellDate.day() == 1) {
        displayText = schedule["title"].toString();
        if (showTime && isStartDay && !isAllDay)
            displayText = start.toString("HH:mm ") + displayText;
    }

    QLabel* label = new QLabel(parent);
    label->setText(displayText);

    QString color = schedule["color"].toString();
    QString style;
    if (isAllDay) {
        QString lr = isStartDay ? "4px" : "0px";
        QString rr = isEndDay   ? "4px" : "0px";
        style = QString("background-color: %1; color: white; border: none; "
                        "border-top-left-radius: %2; border-bottom-left-radius: %2; "
                        "border-top-right-radius: %3; border-bottom-right-radius: %3; "
                        "margin: 0px; padding: 1px; font-size: 10px; font-weight: bold;")
                .arg(color, lr, rr);
    } else {
        style = QString("background-color: transparent; color: %1; border: none; "
                        "font-weight: bold; font-size: 10px; padding: 1px;").arg(color);
        if (!displayText.isEmpty() && !displayText.startsWith("•"))
            label->setText("• " + label->text());
    }
    label->setStyleSheet(style);
    label->setToolTip(QString("%1\n%2 ~ %3").arg(schedule["title"].toString(),
                      start.toString("MM/dd HH:mm"), end.toString("MM/dd HH:mm")));
    return label;
}

static QLabel* makeSpacerLabel(QWidget* parent)
{
    QLabel* sp = new QLabel(parent);
    sp->setFixedHeight(16);
    sp->setStyleSheet("background: transparent; border: none;");
    return sp;
}

void DayCell::setSchedules(const QList<QVariantMap>& schedules) {
    m_currentSchedules = schedules;

    int dateLabelHeight = m_dateLabel->height() > 0 ? m_dateLabel->height() : 22;
    int availableHeight = this->height() - dateLabelHeight - 15;
    int maxSchedules    = qMax(0, availableHeight / 18);
    m_lastMaxSchedules  = maxSchedules;

    bool showTime  = this->width() > 110;
    m_wasTimeShown = showTime;

    this->setUpdatesEnabled(false);
    QLayoutItem* item;
    while ((item = m_scheduleLayout->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }

    if (schedules.isEmpty()) {
        this->setUpdatesEnabled(true);
        this->update();
        return;
    }

    // ── 슬롯을 bar 그룹 / normal 그룹으로 분리 ──
    // bar: multi-day 이벤트(isAllDay) 또는 bar_spacer 마커
    // normal: 단일 날짜 시간 이벤트
    struct SlotEntry { int idx; bool isBar; bool isSpacer; };
    QList<SlotEntry> barEntries, normalEntries;

    for (int i = 0; i < schedules.size(); ++i) {
        const QVariantMap& s = schedules[i];
        if (s.value("_is_bar_spacer").toBool()) {
            barEntries.append({i, true, true});
        } else if (!s.isEmpty()) {
            QDate sS = QDateTime::fromString(s["start"].toString(), "yyyy-MM-dd HH:mm:ss").date();
            QDate sE = QDateTime::fromString(s["end"].toString(),   "yyyy-MM-dd HH:mm:ss").date();
            bool multiDay = (sS != sE);
            if (multiDay)
                barEntries.append({i, true, false});
            else
                normalEntries.append({i, false, false});
        } else {
            // 완전히 빈 슬롯 (normal 자리 공백)
            normalEntries.append({i, false, true});
        }
    }

    // ── 1. 바 그룹: 항상 전부 렌더 (overflow 제외) ──
    for (const auto& e : barEntries) {
        if (e.isSpacer)
            m_scheduleLayout->addWidget(makeSpacerLabel(this));
        else
            m_scheduleLayout->addWidget(makeScheduleLabel(schedules[e.idx], m_date, showTime, this));
    }

    // ── 2. 일반 그룹: 남은 공간에서 overflow 처리 ──
    int usedByBars  = barEntries.size();
    int normalBudget = qMax(0, maxSchedules - usedByBars);

    // 실제 일정(spacer 제외) 중 숨겨질 수
    int hiddenNormal = 0;
    for (int i = normalBudget; i < normalEntries.size(); ++i)
        if (!normalEntries[i].isSpacer) hiddenNormal++;

    bool overflow    = hiddenNormal > 0;
    int  renderNormal = overflow ? qMax(0, normalBudget - 1) : normalBudget;

    for (int i = 0; i < renderNormal && i < normalEntries.size(); ++i) {
        const auto& e = normalEntries[i];
        if (e.isSpacer)
            m_scheduleLayout->addWidget(makeSpacerLabel(this));
        else
            m_scheduleLayout->addWidget(makeScheduleLabel(schedules[e.idx], m_date, showTime, this));
    }

    if (overflow) {
        QLabel* more = new QLabel(QString("+ %1 more").arg(hiddenNormal), this);
        more->setStyleSheet("font-size: 9px; color: gray; border: none;");
        m_scheduleLayout->addWidget(more);
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
