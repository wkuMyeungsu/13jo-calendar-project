#pragma once
#include <QDate>
#include <QList>
#include <QMap>
#include "Schedule.h"
#include "ScheduleSlot.h"

class CalendarEngine {
public:
    // Strict Slotting: Schedule 리스트를 받아 날짜별 ScheduleSlot 배치를 반환
    static QMap<QDate, QList<ScheduleSlot>>
        computeLayout(int year, int month, const QList<Schedule>& schedules);
};
