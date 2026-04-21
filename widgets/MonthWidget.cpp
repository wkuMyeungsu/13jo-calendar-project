#include "MonthWidget.h"
#include "DayCell.h"
#include <QLayoutItem>

static const char* kDayNames[] = { "일", "월", "화", "수", "목", "금", "토" };

MonthWidget::MonthWidget(QWidget* parent) : QWidget(parent) {
    m_gridLayout = new QGridLayout(this);
    m_gridLayout->setSpacing(0);
    m_gridLayout->setContentsMargins(0, 0, 0, 0);

    for (int i = 0; i < UiConstants::DAYS_IN_WEEK; ++i)
        m_gridLayout->setColumnStretch(i, 1);
    m_gridLayout->setRowStretch(0, 0); // 요일 헤더 행
    for (int i = 1; i < (UiConstants::MAX_CALENDAR_CELLS / UiConstants::DAYS_IN_WEEK) + 1; ++i)
        m_gridLayout->setRowStretch(i, 1); // 날짜 행들

    // 요일 헤더 생성 (한 번만 생성)
    for (int col = 0; col < UiConstants::DAYS_IN_WEEK; ++col) {
        QLabel* lbl = new QLabel(kDayNames[col], this);
        lbl->setAlignment(Qt::AlignCenter);
        QString color = (col == 0) ? UiConstants::COLOR_SUN : (col == 6) ? UiConstants::COLOR_SAT : UiConstants::COLOR_WEEKDAY;
        lbl->setStyleSheet(QString(
                               "font-size: %1px; font-weight: bold; color: %2; "
                               "background: #FAFAFA; border-bottom: 1px solid %3; "
                               "padding: 4px 0px;"
                               ).arg(UiConstants::FONT_SIZE_TINY).arg(color).arg(UiConstants::COLOR_BORDER_DEFAULT));
        m_gridLayout->addWidget(lbl, 0, col);
    }

    // [개선] DayCell 42개를 미리 생성하여 풀(Pool)에 저장
    for (int i = 0; i < UiConstants::MAX_CALENDAR_CELLS; ++i) {
        DayCell* cell = new DayCell(this);
        cell->hide(); // 초기에는 숨김
        m_cells.append(cell);

        // 시그널 연결 (한 번만 수행)
        connect(cell, &DayCell::dayClicked, this, [this, cell](const QDate& date) {
            // 모든 셀의 선택 해제
            for (DayCell* c : m_cells) {
                c->setSelected(false);
            }
            cell->setSelected(true);
        });
        connect(cell, &DayCell::dayDoubleClicked, this, &MonthWidget::dayDoubleClicked);
        connect(cell, &DayCell::addRequested,     this, &MonthWidget::addRequested);

        // 그리드에 미리 배치 (요일 헤더가 0행이므로 날짜는 1행부터)
        int row = (i / UiConstants::DAYS_IN_WEEK) + 1;
        int col = i % UiConstants::DAYS_IN_WEEK;
        m_gridLayout->addWidget(cell, row, col);
    }
}

void MonthWidget::setStage(const SafeZoneStage& stage) {
    m_stage = stage;
    for (DayCell* cell : m_cells) {
        cell->setStage(stage);
    }
}

void MonthWidget::updateMonth(int year, int month, const QList<Schedule>& schedules) {
    this->setUpdatesEnabled(false);

    QDate firstDay(year, month, 1);
    const int startCol    = firstDay.dayOfWeek() % UiConstants::DAYS_IN_WEEK;
    const int daysInMonth = firstDay.daysInMonth();

    const QMap<QDate, QList<ScheduleSlot>> daySlots =
        CalendarEngine::computeLayout(year, month, schedules);

    for (int i = 0; i < UiConstants::MAX_CALENDAR_CELLS; ++i) {
        DayCell* cell    = m_cells[i];
        int      dayIndex = i - startCol + 1;
        if (dayIndex >= 1 && dayIndex <= daysInMonth) {
            QDate date(year, month, dayIndex);
            cell->setDate(date);
            cell->setSchedules(daySlots.value(date));
            cell->show();
        } else {
            cell->hide();
        }
    }

    this->setUpdatesEnabled(true);
}