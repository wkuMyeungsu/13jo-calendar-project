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
#include <QVariantMap>
#include <QSettings>
#include <QMessageBox>
#include <QGraphicsDropShadowEffect>
#include <QEvent>
#include <QMouseEvent>
#include <QListWidget>

// ── 프로젝트 전역 상수 및 시스템 (Stable) ──
#include "StyleHelper.h"
#include "../models/DatabaseManager.h"

struct SafeZoneStage {
    int maxSlots;
    int slotHeight;
    int fontSize;
};

inline SafeZoneStage getStageForHeight(int h) {
    if (h < 580)  return {1, 18, 10}; // Stage 1: 최소화 모드 (충돌 방지를 위해 슬롯 높이 18px로 축소)
    if (h < 680)  return {2, 20, 10}; // Stage 2
    if (h < 800)  return {2, 24, 11}; // Stage 3
    if (h < 950)  return {3, 24, 12}; // Stage 4
    if (h < 1100) return {4, 24, 13}; // Stage 5
    return {5, 26, 14};               // Stage 6
}

#endif // UICOMMON_H
