#include "MonthWidget.h"
#include "DayCell.h"
#include <QDateTime>
#include <QLayoutItem>
#include <QSet>
#include <QMap>
#include <algorithm>

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

void MonthWidget::updateMonth(int year, int month, const QList<QVariantMap>& schedules) {
    // [개선] 화면 업데이트를 잠시 중단 (플리커 방지 핵심)
    this->setUpdatesEnabled(false);

    QDate firstDay(year, month, 1);
    int startCol    = firstDay.dayOfWeek() % UiConstants::DAYS_IN_WEEK;
    int daysInMonth = firstDay.daysInMonth();

    // --- 1. 스케줄 슬롯 계산 로직 (기존과 동일하지만 최적화됨) ---
    QMap<QDate, QList<QVariantMap>> daySlots;
    int totalRows = (daysInMonth + startCol + (UiConstants::DAYS_IN_WEEK - 1)) / UiConstants::DAYS_IN_WEEK;

    for (int row = 0; row < totalRows; ++row) {
        QDate weekStart = firstDay.addDays(-startCol + row * UiConstants::DAYS_IN_WEEK);
        QDate weekEnd   = weekStart.addDays(UiConstants::DAYS_IN_WEEK - 1);

        QList<QVariantMap> weekScheds;
        for (const auto& s : schedules) {
            QDate sS = QDateTime::fromString(s["start"].toString(), "yyyy-MM-dd HH:mm:ss").date();
            QDate sE = QDateTime::fromString(s["end"].toString(),   "yyyy-MM-dd HH:mm:ss").date();
            if (sS <= weekEnd && sE >= weekStart) weekScheds.append(s);
        }

        // 정렬
        std::sort(weekScheds.begin(), weekScheds.end(), [&](const QVariantMap& a, const QVariantMap& b) {
            QDate aS = QDateTime::fromString(a["start"].toString(), "yyyy-MM-dd HH:mm:ss").date();
            QDate aE = QDateTime::fromString(a["end"].toString(),   "yyyy-MM-dd HH:mm:ss").date();
            QDate bS = QDateTime::fromString(b["start"].toString(), "yyyy-MM-dd HH:mm:ss").date();
            QDate bE = QDateTime::fromString(b["end"].toString(),   "yyyy-MM-dd HH:mm:ss").date();
            QDate aClamp = qMax(aS, weekStart);
            QDate bClamp = qMax(bS, weekStart);
            if (aClamp != bClamp) return aClamp < bClamp;
            return aS.daysTo(aE) > bS.daysTo(bE);
        });

        // 슬롯 배정
        QList<QSet<int>> slotOccupied;
        QList<QPair<QVariantMap, int>> weekAssignments;
        for (const auto& s : weekScheds) {
            QDate sS = QDateTime::fromString(s["start"].toString(), "yyyy-MM-dd HH:mm:ss").date();
            QDate sE = QDateTime::fromString(s["end"].toString(),   "yyyy-MM-dd HH:mm:ss").date();
            QDate clampS = qMax(sS, weekStart);
            QDate clampE = qMin(sE, weekEnd);
            QSet<int> sDays;
            for (QDate d = clampS; d <= clampE; d = d.addDays(1)) sDays.insert((int)weekStart.daysTo(d));
            int assigned = -1;
            for (int slot = 0; slot < slotOccupied.size(); ++slot) {
                if ((slotOccupied[slot] & sDays).isEmpty()) { assigned = slot; break; }
            }
            if (assigned == -1) { assigned = slotOccupied.size(); slotOccupied.append(QSet<int>()); }
            slotOccupied[assigned] |= sDays;
            weekAssignments.append(qMakePair(s, assigned));
        }

        int totalSlots = slotOccupied.size();
        QSet<int> barSlotIndices;
        for (const auto& ass : weekAssignments) {
            if (QDateTime::fromString(ass.first["start"].toString(), "yyyy-MM-dd HH:mm:ss").date() !=
                QDateTime::fromString(ass.first["end"].toString(), "yyyy-MM-dd HH:mm:ss").date())
                barSlotIndices.insert(ass.second);
        }

        for (int col = 0; col < UiConstants::DAYS_IN_WEEK; ++col) {
            QDate date = weekStart.addDays(col);
            if (date.month() != month) continue;
            QList<QVariantMap> slotList(totalSlots);
            for (const auto& ass : weekAssignments) {
                if (date >= QDateTime::fromString(ass.first["start"].toString(), "yyyy-MM-dd HH:mm:ss").date() &&
                    date <= QDateTime::fromString(ass.first["end"].toString(), "yyyy-MM-dd HH:mm:ss").date())
                    slotList[ass.second] = ass.first;
            }
            for (int i = 0; i < totalSlots; ++i) {
                if (slotList[i].isEmpty() && barSlotIndices.contains(i)) {
                    QVariantMap spacer; spacer["_is_bar_spacer"] = true; slotList[i] = spacer;
                }
            }
            daySlots[date] = slotList;
        }
    }

    // --- 2. [개선] 미리 만들어둔 Cell 업데이트 및 노출 제어 ---
    // 모든 셀을 돌며 이번 달에 속하는지 확인
    for (int i = 0; i < UiConstants::MAX_CALENDAR_CELLS; ++i) {
        DayCell* cell = m_cells[i];
        int dayIndex = i - startCol + 1; // 실제 날짜 계산

        if (dayIndex >= 1 && dayIndex <= daysInMonth) {
            // 이번 달 날짜에 해당하는 경우
            QDate date(year, month, dayIndex);

            // 데이터 설정 (DayCell 내부에 변경이 있을 때만 그리도록 최적화 필요)
            cell->setDate(date);
            cell->setSchedules(daySlots.value(date));
            cell->show();
        } else {
            // 이번 달이 아닌 칸은 숨김 (또는 이전/다음 달 날짜로 표시 가능)
            cell->hide();
        }
    }

    // [개선] 업데이트 재개 (배치된 결과만 한 번에 그림)
    this->setUpdatesEnabled(true);
}