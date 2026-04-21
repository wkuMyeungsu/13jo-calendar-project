#ifndef STYLEHELPER_H
#define STYLEHELPER_H

#include <QString>

namespace StyleHelper {
    // ── 공통 수치 상항 ──
    inline const int WIDGET_WIDTH = 400;
    inline const int WIDGET_HEIGHT = 550;
    inline const int CONTENT_MARGIN = 20;
    inline const int LAYOUT_SPACING = 15;

    // ── 공통 텍스트 스타일 ──
    inline const QString HEADER_STYLE = "font-size: 18px; font-weight: bold; color: #333; margin-bottom: 5px;";
    inline const QString FORM_LABEL_STYLE = "color: #666; font-weight: bold; font-size: 13px;";

    // ── 공통 입력 필드 스타일 (LineEdit, Combo, DateTime, TextEdit) ──
    inline const QString COMMON_INPUT_STYLE = 
        "QLineEdit, QDateTimeEdit, QComboBox, QTextEdit {"
        "  border: 1px solid #DDD; border-radius: 6px; padding: 8px; padding-right: 30px; background: #FDFDFD; color: #333;"
        "}"
        "QLineEdit:focus, QDateTimeEdit:focus, QComboBox:focus, QTextEdit:focus {"
        "  border: 1px solid #4A90E2; background: white;"
        "}"
        "QComboBox::drop-down, QDateTimeEdit::drop-down {"
        "  subcontrol-origin: padding; subcontrol-position: top right; width: 24px; "
        "  border-left: 1px solid #EEE; border-top-right-radius: 6px; border-bottom-right-radius: 6px; background: #F5F5F5;"
        "}"
        "QComboBox::down-arrow, QDateTimeEdit::down-arrow {"
        "  width: 0; height: 0; border-left: 4px solid transparent; border-right: 4px solid transparent; border-top: 5px solid #888;"
        "  position: relative; top: 1px; right: 2px;"
        "}";

    // ── 공통 체크박스 스타일 ──
    inline const QString CHECKBOX_STYLE = 
        "QCheckBox { color: #555; font-weight: bold; }"
        "QCheckBox::indicator { width: 18px; height: 18px; }";

    // ── 공통 스크롤바 스타일 (TextEdit, ScrollArea 전용) ──
    inline const QString SCROLLBAR_STYLE = 
        "QScrollBar:vertical { border: none; background: transparent; width: 8px; margin: 2px; }"
        "QScrollBar::handle:vertical { background: #E0E0E0; min-height: 20px; border-radius: 4px; }"
        "QScrollBar::handle:vertical:hover { background: #BDBDBD; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; }"
        "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { background: none; }";

    // ── 날짜 선택기(Calendar) 팝업 스타일 ──
    inline const QString CALENDAR_POPUP_STYLE = 
        "QCalendarWidget QWidget { background-color: white; }"
        "QCalendarWidget QToolButton { color: #333; font-weight: bold; background-color: transparent; border: none; padding: 5px; }"
        "QCalendarWidget QToolButton:hover { background-color: #EEE; }"
        "QCalendarWidget QMenu { background-color: white; color: #333; border: 1px solid #DDD; }"
        "QCalendarWidget QSpinBox { color: #333; background-color: white; border: 1px solid #DDD; padding-right: 15px; }"
        "QCalendarWidget QAbstractItemView:enabled { color: #333; selection-background-color: #4A90E2; selection-color: white; }";

    // ── 버튼 테마 스타일 ──
    inline const QString BTN_SAVE_STYLE = 
        "QPushButton { background-color: #4CAF50; color: white; padding: 12px; font-size: 14px; font-weight: bold; border-radius: 6px; border: none; }"
        "QPushButton:hover { background-color: #45a049; }";

    inline const QString BTN_MODIFY_STYLE = 
        "QPushButton { background-color: #4A90E2; color: white; padding: 12px; font-size: 14px; font-weight: bold; border-radius: 6px; border: none; }"
        "QPushButton:hover { background-color: #357ABD; }"
        "QPushButton:disabled { background-color: #E0E0E0; color: #AAA; }";

    inline const QString BTN_DELETE_STYLE = 
        "QPushButton { background-color: #F8D7DA; color: #721C24; padding: 12px; font-size: 14px; font-weight: bold; border-radius: 6px; border: 1px solid #F5C6CB; }"
        "QPushButton:hover { background-color: #E24A4A; color: white; border-color: #E24A4A; }"
        "QPushButton:disabled { background-color: #F9F9F9; color: #DDD; border-color: #EEE; }";

    // ── 리스트 위젯 스타일 (CategoryModify 전용) ──
    inline const QString LIST_WIDGET_STYLE = 
        "QListWidget { border: 1px solid #EEE; border-radius: 8px; background: #FAFAFA; padding: 5px; }"
        "QListWidget::item { background: white; border: 1px solid #EEE; border-radius: 6px; padding: 10px; margin-bottom: 2px; }"
        "QListWidget::item:selected { background: #F0F7FF; border: 1px solid #4A90E2; color: #333; }"
        "QListWidget::item:hover { background: #F5F9FF; border: 1px solid #D0E5FF; }"
        "QScrollBar:vertical { border: none; background: transparent; width: 8px; margin: 0px; }"
        "QScrollBar::handle:vertical { background: #E0E0E0; min-height: 20px; border-radius: 4px; }"
        "QScrollBar::handle:vertical:hover { background: #BDBDBD; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; }";

    // ── 일정 항목 카드 스타일 (ScheduleManager 전용) ──
    inline const QString ITEM_BASE_STYLE = 
        "#itemWidget { background-color: white; border-left: 5px solid %1; border-radius: 6px; border-top: 1px solid #EEE; border-right: 1px solid #EEE; border-bottom: 1px solid #EEE; }";
    
    inline const QString ITEM_HOVER_STYLE = 
        "#itemWidget { background-color: #F5F9FF; border-left: 5px solid %1; border-top: 1px solid #4A90E2; border-right: 1px solid #4A90E2; border-bottom: 1px solid #4A90E2; }";
}

#endif // STYLEHELPER_H
