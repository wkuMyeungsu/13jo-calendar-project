#include "CalendarEngine.h"
#include <QSet>
#include <algorithm>

QMap<QDate, QList<ScheduleSlot>>
CalendarEngine::computeLayout(int year, int month, const QList<Schedule>& schedules) {
    QMap<QDate, QList<ScheduleSlot>> result;

    QDate firstDay(year, month, 1);
    const int daysInMonth = firstDay.daysInMonth();
    const int startCol    = firstDay.dayOfWeek() % 7; // 0 = 일요일
    const int totalRows   = (daysInMonth + startCol + 6) / 7;

    for (int row = 0; row < totalRows; ++row) {
        QDate weekStart = firstDay.addDays(-startCol + row * 7);
        QDate weekEnd   = weekStart.addDays(6);

        // 이 주에 걸치는 일정만 추출
        QList<Schedule> weekScheds;
        for (const auto& s : schedules) {
            if (s.start.date() <= weekEnd && s.end.date() >= weekStart)
                weekScheds.append(s);
        }

        // 정렬: 주 내 시작일 빠른 순, 같으면 기간 긴 순
        std::sort(weekScheds.begin(), weekScheds.end(), [&](const Schedule& a, const Schedule& b) {
            QDate aClamp = qMax(a.start.date(), weekStart);
            QDate bClamp = qMax(b.start.date(), weekStart);
            if (aClamp != bClamp) return aClamp < bClamp;
            return a.start.date().daysTo(a.end.date()) > b.start.date().daysTo(b.end.date());
        });

        // 슬롯 배정 (그리디: 첫 번째 비어있는 슬롯)
        QList<QSet<int>> slotOccupied;
        QList<QPair<Schedule, int>> weekAssignments;
        for (const auto& s : weekScheds) {
            QDate clampS = qMax(s.start.date(), weekStart);
            QDate clampE = qMin(s.end.date(),   weekEnd);
            QSet<int> sDays;
            for (QDate d = clampS; d <= clampE; d = d.addDays(1))
                sDays.insert((int)weekStart.daysTo(d));

            int assigned = -1;
            for (int i = 0; i < slotOccupied.size(); ++i) {
                if ((slotOccupied[i] & sDays).isEmpty()) { assigned = i; break; }
            }
            if (assigned == -1) {
                assigned = slotOccupied.size();
                slotOccupied.append(QSet<int>());
            }
            slotOccupied[assigned] |= sDays;
            weekAssignments.append(qMakePair(s, assigned));
        }

        const int totalSlots = slotOccupied.size();

        // 멀티데이 바(bar)가 차지하는 슬롯 인덱스 기록 (BarSpacer 삽입 기준)
        QSet<int> barSlotIndices;
        for (const auto& ass : weekAssignments) {
            if (ass.first.start.date() != ass.first.end.date())
                barSlotIndices.insert(ass.second);
        }

        // 날짜별 슬롯 리스트 구성
        for (int col = 0; col < 7; ++col) {
            QDate date = weekStart.addDays(col);
            if (date.month() != month) continue;

            QList<ScheduleSlot> slotList(totalSlots);
            for (const auto& ass : weekAssignments) {
                if (date >= ass.first.start.date() && date <= ass.first.end.date())
                    slotList[ass.second] = { ScheduleSlot::Kind::Schedule, ass.first };
            }
            for (int i = 0; i < totalSlots; ++i) {
                if (slotList[i].kind == ScheduleSlot::Kind::Empty && barSlotIndices.contains(i))
                    slotList[i].kind = ScheduleSlot::Kind::BarSpacer;
            }
            result[date] = slotList;
        }
    }

    return result;
}
