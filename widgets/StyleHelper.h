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

    inline QString getDayCellHoverBg() {
        switch (currentTheme) {
            case Theme::Soft:     return "#F3E5F5";
            case Theme::DeepBlue: return "#DCEBFA";
            case Theme::Default: 
            default:              return "#F0F7FF";
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

    // ── 오늘 날짜 강조 스타일 ──
    inline QString getTodayHighlightStyle() {
        return "background-color: #FF9800; color: white; border-radius: 13px; font-weight: bold; border: none; margin-left: 4px;";
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
            "QPushButton:disabled { background-color: #E0E0E0; color: #9E9E9E; }"
        ).arg(color, hover);
    }

    inline QString getBtnModifyStyle() {
        QString primary = getPrimaryColor();
        return QString(
            "QPushButton { background-color: %1; color: white; padding: 12px; font-size: 14px; font-weight: bold; border-radius: 6px; border: none; }"
            "QPushButton:hover { opacity: 0.8; }"
            "QPushButton:disabled { background-color: #F5F5F5; color: #BDBDBD; border: 1px solid #E0E0E0; }"
        ).arg(primary);
    }

    inline QString getBtnDeleteStyle() {
        return "QPushButton { background-color: #F8D7DA; color: #721C24; padding: 12px; font-size: 14px; font-weight: bold; border-radius: 6px; border: 1px solid #F5C6CB; }"
               "QPushButton:hover { background-color: #E24A4A; color: white; border-color: #E24A4A; }"
               "QPushButton:disabled { background-color: #FAFAFA; color: #EF9A9A; border: 1px solid #F5F5F5; }";
    }

    // ── 원형 색상 선택 버튼 스타일 ──
    inline QString getCircleButtonStyle(const QString& color, int size = 32) {
        return QString(
            "QPushButton { background-color: %1; border-radius: %2px; border: 2px solid white; outline: 1px solid #DDD; }"
            "QPushButton:hover { border: 2px solid #999; }"
            "QPushButton:disabled { background-color: #EEE; outline: 1px solid #EEE; border: 2px solid #F5F5F5; }"
        ).arg(color).arg(size / 2);
    }

    // ── 다이얼로그 전체 프레임 스타일 (통합 테두리 및 곡률) ──
    inline QString getDialogFrameStyle() {
        return QString("QFrame#mainFrame { background-color: %1; border: 1px solid #DDD; border-radius: 12px; }").arg(getBgColor());
    }

    // ── 다이얼로그 내부 컨텐츠 스타일 (테두리 제거) ──
    inline QString getDialogStyle() {
        return "QWidget#container { background-color: transparent; border: none; }";
    }

    // ── 팝업 스타일 (ColorPicker 등) ──
    inline QString getPopupStyle() {
        QString bg = getBgColor();
        QString border = (currentTheme == Theme::DeepBlue ? "#ADCCFB" : "#DDD");
        return QString("QWidget#container { background-color: %1; border: 1px solid %2; border-radius: 8px; }").arg(bg, border);
    }

    // ── 커스텀 타이틀바 전용 스타일 (상단 곡률만 유지, 테두리 제거) ──
    inline QString getTitleBarStyle(const QString& bgColor, const QString& borderColor) {
        return QString(
            "#customTitleBar { background-color: %1; border: none; border-bottom: 1px solid %2; border-top-left-radius: 11px; border-top-right-radius: 11px; }"
        ).arg(bgColor, borderColor);
    }

    // ── 카테고리 알약(Pill) 스타일 ──
    inline QString getCategoryPillStyle(const QString& color, bool checked) {
        QString bgColor = getBgColor();
        if (checked) {
            return QString("QPushButton { border: 1px solid %1; border-radius: 12px; padding: 0px 12px; font-size: 11px; color: white; background-color: %1; font-weight: bold; }").arg(color);
        } else {
            return QString("QPushButton { border: 1px solid %1; border-radius: 12px; padding: 0px 12px; font-size: 11px; color: %1; background-color: %2; font-weight: bold; opacity: 0.5; }").arg(color, bgColor);
        }
    }

    // ── 단일일 올데이 바 스타일 ──
    inline QString getAllDayBarStyle(const QString& color) {
        return QString("background-color: %1; color: white; border-radius: 3px; font-size: 11px; font-weight: bold; margin: 1px 2px; padding-left: 4px;").arg(color);
    }

    inline QString getItemBaseStyle(const QString& catColor) {
        QString border = (currentTheme == Theme::DeepBlue ? "#D2E3FC" : (currentTheme == Theme::Soft ? "#E1BEE7" : "#EEE"));
        return QString("#itemWidget { background-color: white; border-radius: 6px; border: 1px solid %2; border-left: 5px solid %1; }").arg(catColor, border);
    }

    inline QString getItemHoverStyle(const QString& catColor) {
        QString bg = (currentTheme == Theme::DeepBlue ? "#E8F0FE" : (currentTheme == Theme::Soft ? "#F3E5F5" : "#F8F9FA"));
        QString primary = getPrimaryColor();
        return QString("#itemWidget { background-color: %1; border-radius: 6px; border: 1px solid %3; border-left: 5px solid %2; }").arg(bg, catColor, primary);
    }

    // ── 카테고리 필터 위젯 전용 스타일 ──
    inline QString getAddButtonStyle() {
        QString primary = getPrimaryColor();
        return QString(
            "QPushButton { border-radius: 11px; background-color: %1; color: white; font-weight: bold; font-size: 16px; border: none; }"
            "QPushButton:hover { background-color: %2; }"
        ).arg(primary, QColor(primary).lighter(110).name());
    }

    inline QString getMoreButtonStyle() {
        QString border = (currentTheme == Theme::DeepBlue ? "#ADCCFB" : "#DDD");
        return QString(
            "QPushButton { border: 1px solid %1; border-radius: 4px; background-color: #F8F9FA; color: #666; font-weight: bold; font-size: 14px; }"
            "QPushButton:hover { background-color: #EEE; }"
            "QPushButton:checked { background-color: #E0E0E0; border: 1px solid #999; }"
        ).arg(border);
    }

    inline QString getOverflowPanelStyle() {
        QString border = (currentTheme == Theme::DeepBlue ? "#ADCCFB" : "#EEE");
        return QString("QWidget { background-color: #F8F9FA; border-top: 1px solid %1; border-bottom: 1px solid %1; }").arg(border);
    }

    // ── 미니 모드 전용 스타일 ──
    inline QString getMiniModeContentStyle() {
        return QString("QWidget#miniContent { background-color: %1; border-radius: 12px; }").arg(getBgColor());
    }

    inline QString getMiniTimeStyle() {
        return QString("font-size: 32px; font-weight: bold; color: %1; background: transparent;").arg(getPrimaryColor());
    }

    inline QString getMiniDateStyle() {
        return QString("font-weight: bold; font-size: 15px; color: %1; background: transparent;").arg(getTextColor());
    }

    inline QString getPinButtonStyle() {
        return "QPushButton { background: transparent; border: 1px solid #DDD; border-radius: 4px; font-size: 14px; }"
               "QPushButton:hover { background: #EEE; }"
               "QPushButton:checked { background: #E3F2FD; border: 1px solid #2196F3; }";
    }
}

#endif // STYLEHELPER_H
