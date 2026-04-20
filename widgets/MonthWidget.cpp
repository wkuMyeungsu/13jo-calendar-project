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

    for (int i = 0; i < 7; ++i)
        m_gridLayout->setColumnStretch(i, 1);
    m_gridLayout->setRowStretch(0, 0);
    for (int i = 1; i <= 6; ++i)
        m_gridLayout->setRowStretch(i, 1);

    for (int col = 0; col < 7; ++col) {
        QLabel* lbl = new QLabel(kDayNames[col], this);
        lbl->setAlignment(Qt::AlignCenter);
        QString color = (col == 0) ? "#D32F2F" : (col == 6) ? "#1565C0" : "#757575";
        lbl->setStyleSheet(QString(
            "font-size: 11px; font-weight: bold; color: %1; "
            "background: #FAFAFA; border-bottom: 1px solid #E0E0E0; "
            "padding: 4px 0px;"
        ).arg(color));
        m_gridLayout->addWidget(lbl, 0, col);
    }
}

void MonthWidget::updateMonth(int year, int month, const QList<QVariantMap>& schedules) {
    for (DayCell* cell : m_cells) delete cell;
    m_cells.clear();

    QDate firstDay(year, month, 1);
    int startCol    = firstDay.dayOfWeek() % 7; // 0=일, 6=토
    int daysInMonth = firstDay.daysInMonth();
    int totalRows   = (daysInMonth + startCol + 6) / 7;

    // ── 2. 주 단위 슬롯 계산 → 날짜별 슬롯 리스트 구성 ──
    // daySlots[date] = QList<QVariantMap> 에서 index == 슬롯 번호
    // 빈 QVariantMap(.isEmpty()==true) == 해당 슬롯 공백(스페이서)
    QMap<QDate, QList<QVariantMap>> daySlots;

    for (int row = 0; row < totalRows; ++row) {
        QDate weekStart = firstDay.addDays(-startCol + row * 7);
        QDate weekEnd   = weekStart.addDays(6);

        // 이 주에 걸친 일정 추려서 "이번 주 시작일 빠른 순 → 기간 긴 순" 정렬
        QList<QVariantMap> weekScheds;
        for (const auto& s : schedules) {
            QDate sS = QDateTime::fromString(s["start"].toString(), "yyyy-MM-dd HH:mm:ss").date();
            QDate sE = QDateTime::fromString(s["end"].toString(),   "yyyy-MM-dd HH:mm:ss").date();
            if (sS <= weekEnd && sE >= weekStart)
                weekScheds.append(s);
        }
        std::sort(weekScheds.begin(), weekScheds.end(), [&](const QVariantMap& a, const QVariantMap& b) {
            QDate aS = QDateTime::fromString(a["start"].toString(), "yyyy-MM-dd HH:mm:ss").date();
            QDate aE = QDateTime::fromString(a["end"].toString(),   "yyyy-MM-dd HH:mm:ss").date();
            QDate bS = QDateTime::fromString(b["start"].toString(), "yyyy-MM-dd HH:mm:ss").date();
            QDate bE = QDateTime::fromString(b["end"].toString(),   "yyyy-MM-dd HH:mm:ss").date();
            QDate aClamp = qMax(aS, weekStart);
            QDate bClamp = qMax(bS, weekStart);
            if (aClamp != bClamp) return aClamp < bClamp; // 이번 주 시작일 빠른 순
            int aDur = aS.daysTo(aE);
            int bDur = bS.daysTo(bE);
            return aDur > bDur; // 기간 긴 순
        });

        // 슬롯 탐욕 배정: slotOccupied[slot] = 이 주에서 점유된 상대 요일 인덱스 집합(0-6)
        QList<QSet<int>> slotOccupied;
        QList<QPair<QVariantMap, int>> weekAssignments; // (일정, 슬롯)

        for (const auto& s : weekScheds) {
            QDate sS = QDateTime::fromString(s["start"].toString(), "yyyy-MM-dd HH:mm:ss").date();
            QDate sE = QDateTime::fromString(s["end"].toString(),   "yyyy-MM-dd HH:mm:ss").date();
            QDate clampS = qMax(sS, weekStart);
            QDate clampE = qMin(sE, weekEnd);

            QSet<int> sDays;
            for (QDate d = clampS; d <= clampE; d = d.addDays(1))
                sDays.insert((int)weekStart.daysTo(d));

            int assigned = -1;
            for (int slot = 0; slot < slotOccupied.size(); ++slot) {
                if ((slotOccupied[slot] & sDays).isEmpty()) {
                    assigned = slot;
                    break;
                }
            }
            if (assigned == -1) {
                assigned = slotOccupied.size();
                slotOccupied.append(QSet<int>());
            }
            slotOccupied[assigned] |= sDays;
            weekAssignments.append(qMakePair(s, assigned));
        }

        int totalSlots = slotOccupied.size();

        // 바(multi-day) 이벤트가 점유한 슬롯 인덱스 수집
        QSet<int> barSlotIndices;
        for (const auto& assignment : weekAssignments) {
            QDate sS = QDateTime::fromString(assignment.first["start"].toString(), "yyyy-MM-dd HH:mm:ss").date();
            QDate sE = QDateTime::fromString(assignment.first["end"].toString(),   "yyyy-MM-dd HH:mm:ss").date();
            if (sS != sE)
                barSlotIndices.insert(assignment.second);
        }

        // 날짜별 슬롯 리스트 생성
        for (int col = 0; col < 7; ++col) {
            QDate date = weekStart.addDays(col);
            if (date.month() != month) continue;

            QList<QVariantMap> slotList(totalSlots);
            for (const auto& assignment : weekAssignments) {
                const QVariantMap& sched = assignment.first;
                int slot = assignment.second;
                QDate sS = QDateTime::fromString(sched["start"].toString(), "yyyy-MM-dd HH:mm:ss").date();
                QDate sE = QDateTime::fromString(sched["end"].toString(),   "yyyy-MM-dd HH:mm:ss").date();
                if (date >= sS && date <= sE)
                    slotList[slot] = sched;
            }
            // 빈 bar 슬롯 → bar spacer 마킹 (DayCell이 항상 렌더)
            for (int i = 0; i < totalSlots; ++i) {
                if (slotList[i].isEmpty() && barSlotIndices.contains(i)) {
                    QVariantMap spacer;
                    spacer["_is_bar_spacer"] = true;
                    slotList[i] = spacer;
                }
            }
            daySlots[date] = slotList;
        }
    }

    // ── 3. DayCell 생성 ──
    for (int day = 1; day <= daysInMonth; ++day) {
        DayCell* cell = new DayCell(this);
        QDate date(year, month, day);
        cell->setDate(date);
        cell->setSchedules(daySlots.value(date));

        connect(cell, &DayCell::dayDoubleClicked, this, &MonthWidget::dayDoubleClicked);
        connect(cell, &DayCell::addRequested,     this, &MonthWidget::addRequested);

        int row = (day + startCol - 1) / 7 + 1;
        int col = (day + startCol - 1) % 7;
        m_gridLayout->addWidget(cell, row, col);
        m_cells.append(cell);
    }
}
