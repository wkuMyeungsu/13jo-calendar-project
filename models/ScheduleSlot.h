#pragma once
#include "Schedule.h"

struct ScheduleSlot {
    enum class Kind { Empty, BarSpacer, Schedule };
    Kind     kind = Kind::Empty;
    ::Schedule data; // valid when kind == Kind::Schedule
};
