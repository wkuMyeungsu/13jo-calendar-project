#ifndef STYLEHELPER_H
#define STYLEHELPER_H

#include <QString>

namespace StyleHelper {
    // ── 테마 정의 ──
    enum class Theme {
        Default,  // 깔끔한 화이트 & 블루
        Soft,     // 연한 파스텔 보라
        DeepBlue  // 신뢰감 있는 딥 블루
    };

    // 현재 활성화된 테마
    inline Theme currentTheme = Theme::Default;

    // ── 공통 수치 상항 ──
    inline const int WIDGET_WIDTH = 400;
    inline const int WIDGET_HEIGHT = 550;
    inline const int CONTENT_MARGIN = 20;
    inline const int LAYOUT_SPACING = 15;

    // ── 테마별 색상 추출 함수 ──
    inline QString getPrimaryColor() {
        switch (currentTheme) {
            case Theme::Soft:     return "#9575CD";
            case Theme::DeepBlue: return "#1A73E8";
            case Theme::Default: 
            default:              return "#4A90E2";
        }
    }

    inline QString getBgColor() {
        switch (currentTheme) {
            case Theme::Soft:     return "#FDF7FF";
            case Theme::DeepBlue: return "#E8F0FE";
            case Theme::Default: 
            default:              return "white";
        }
    }

    inline QString getTextColor() {
        switch (currentTheme) {
            case Theme::Soft:     return "#5D4037";
            case Theme::DeepBlue: return "#174EA6";
            case Theme::Default: 
            default:              return "#333333";
        }
    }

    // ── 통합 스타일 생성 함수들 ──

    inline QString getHeaderStyle() {
        return QString("font-size: 18px; font-weight: bold; color: %1; border: none; background: transparent;").arg(getTextColor());
    }

    inline QString getFormLabelStyle() {
        QString color = (currentTheme == Theme::DeepBlue) ? "#1967D2" : "#666";
        return QString("font-size: 13px; font-weight: bold; color: %1; border: none; background: transparent;").arg(color);
    }

    inline QString getCheckboxStyle() {
        return QString("QCheckBox { color: %1; font-weight: bold; } QCheckBox::indicator { width: 18px; height: 18px; }").arg(getTextColor());
    }

    inline QString getListWidgetStyle() {
        QString border = (currentTheme == Theme::DeepBlue ? "#ADCCFB" : "#DDD");
        QString bg = (currentTheme == Theme::DeepBlue ? "#F1F6FF" : "white");
        return QString("QListWidget { border: 1px solid %1; border-radius: 8px; background: %2; outline: none; padding: 5px; }").arg(border, bg);
    }

    inline QString getCommonInputStyle() {
        QString primary = getPrimaryColor();
        QString bg = "#FDFDFD";
        QString text = getTextColor();
        QString border = (currentTheme == Theme::DeepBlue ? "#ADCCFB" : "#DDD");

        return QString(
            "QLineEdit, QDateTimeEdit, QComboBox, QTextEdit, QSpinBox {"
            "  border: 1px solid %1; border-radius: 6px; padding: 8px; padding-right: 30px; background: %2; color: %3;"
            "}"
            "QLineEdit:focus, QDateTimeEdit:focus, QComboBox:focus, QTextEdit:focus, QSpinBox:focus {"
            "  border: 1px solid %4; background: white;"
            "}"
            "QComboBox::drop-down, QDateTimeEdit::drop-down {"
            "  subcontrol-origin: padding; subcontrol-position: top right; width: 24px; "
            "  border-left: 1px solid %1; border-top-right-radius: 6px; border-bottom-right-radius: 6px; background: #F5F5F5;"
            "}"
            "QComboBox::down-arrow, QDateTimeEdit::down-arrow {"
            "  width: 0; height: 0; border-left: 4px solid transparent; border-right: 4px solid transparent; border-top: 5px solid #888;"
            "  position: relative; top: 1px; right: 2px;"
            "}"
        ).arg(border, bg, text, primary);
    }

    inline QString getScrollbarStyle() {
        QString handle = (currentTheme == Theme::DeepBlue ? "#AECBFA" : "#E0E0E0");
        QString hover = (currentTheme == Theme::DeepBlue ? "#8AB4F8" : "#BDBDBD");
        return QString(
            "QScrollBar:vertical { border: none; background: transparent; width: 8px; margin: 0px; }"
            "QScrollBar::handle:vertical { background: %1; min-height: 20px; border-radius: 4px; }"
            "QScrollBar::handle:vertical:hover { background: %2; }"
            "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; }"
            "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { background: none; }"
        ).arg(handle, hover);
    }

    inline QString getCalendarPopupStyle() {
        QString text = getTextColor();
        QString primary = getPrimaryColor();
        QString bg = getBgColor();
        return QString(
            "QCalendarWidget QWidget { background-color: %1; }"
            "QCalendarWidget QToolButton { color: %2; font-weight: bold; background-color: transparent; border: none; padding: 5px; }"
            "QCalendarWidget QToolButton:hover { background-color: #EEE; }"
            "QCalendarWidget QMenu { background-color: %1; color: %2; border: 1px solid #DDD; }"
            "QCalendarWidget QSpinBox { color: %2; background-color: %1; border: 1px solid #DDD; padding-right: 15px; }"
            "QCalendarWidget QAbstractItemView:enabled { color: %2; selection-background-color: %3; selection-color: white; }"
        ).arg(bg, text, primary);
    }

    inline QString getBtnSaveStyle() {
        QString color = (currentTheme == Theme::Soft) ? "#AB47BC" : (currentTheme == Theme::DeepBlue ? "#1967D2" : "#4CAF50");
        QString hover = (currentTheme == Theme::Soft) ? "#8E24AA" : (currentTheme == Theme::DeepBlue ? "#174EA6" : "#45a049");
        return QString(
            "QPushButton { background-color: %1; color: white; padding: 12px; font-size: 14px; font-weight: bold; border-radius: 6px; border: none; }"
            "QPushButton:hover { background-color: %2; }"
        ).arg(color, hover);
    }

    inline QString getBtnModifyStyle() {
        return QString(
            "QPushButton { background-color: %1; color: white; padding: 12px; font-size: 14px; font-weight: bold; border-radius: 6px; border: none; }"
            "QPushButton:hover { opacity: 0.8; }"
        ).arg(getPrimaryColor());
    }

    inline QString getBtnDeleteStyle() {
        return "QPushButton { background-color: #F8D7DA; color: #721C24; padding: 12px; font-size: 14px; font-weight: bold; border-radius: 6px; border: 1px solid #F5C6CB; }"
               "QPushButton:hover { background-color: #E24A4A; color: white; border-color: #E24A4A; }";
    }

    inline QString getItemBaseStyle(const QString& catColor) {
        QString border = (currentTheme == Theme::DeepBlue ? "#D2E3FC" : "#EEE");
        return QString("#itemWidget { background-color: white; border-left: 5px solid %1; border-radius: 6px; border-top: 1px solid %2; border-right: 1px solid %2; border-bottom: 1px solid %2; }").arg(catColor, border);
    }

    inline QString getItemHoverStyle(const QString& catColor) {
        QString bg = (currentTheme == Theme::DeepBlue ? "#F1F6FF" : "#F5F9FF");
        return QString("#itemWidget { background-color: %1; border-left: 5px solid %2; border-radius: 6px; border-top: 1px solid %3; border-right: 1px solid %3; border-bottom: 1px solid %3; }").arg(bg, catColor, getPrimaryColor());
    }
}

#endif // STYLEHELPER_H
