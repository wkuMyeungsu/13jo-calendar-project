#ifndef UICOMMON_H
#define UICOMMON_H

// ── Qt 핵심 프레임워크 (Stable Headers) ──
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QTextEdit>
#include <QDateTimeEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QScrollArea>
#include <QDate>
#include <QDateTime>
#include <QTime>
#include <QTimer>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QPoint>
#include <QDebug>
#include <QSettings>
#include <QGraphicsDropShadowEffect>
#include <QEvent>
#include <QMouseEvent>
#include <QListWidget>

// ── 프로젝트 전역 상수 및 시스템 (Stable) ──
#include "StyleHelper.h"
#include "UiConstants.h"
#include "../models/DatabaseManager.h"
#include "../models/ScheduleSlot.h"

struct SafeZoneStage {
    int maxSlots;
    int slotHeight;
    int fontSize;
    int dateHeight;   // 날짜 표시 영역 높이
    int dateFontSize; // 날짜 폰트 크기

    bool operator==(const SafeZoneStage& other) const {
        return maxSlots == other.maxSlots && slotHeight == other.slotHeight && fontSize == other.fontSize &&
               dateHeight == other.dateHeight && dateFontSize == other.dateFontSize;
    }
    bool operator!=(const SafeZoneStage& other) const {
        return !(*this == other);
    }
};

inline SafeZoneStage getStageForHeight(int h) {
    if (h < 580)  return {2, 16, 9, 14, 9};  // Stage 1: 날짜 최소화
    if (h < 720)  return {3, 18, 10, 16, 10}; // Stage 2
    if (h < 880)  return {4, 20, 11, 18, 11}; // Stage 3
    if (h < 1050) return {5, 22, 12, 20, 12}; // Stage 4
    return {6, 24, 13, 22, 13};               // Stage 5
}

#endif // UICOMMON_H
