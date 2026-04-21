#ifndef UICONSTANTS_H
#define UICONSTANTS_H

#include <QString>

namespace UiConstants {
    // Window Constants
    static constexpr int DEFAULT_MIN_WIDTH = 400;
    static constexpr int DEFAULT_MIN_HEIGHT = 500;
    static constexpr int TITLE_BAR_HEIGHT = 30;
    static constexpr int BASE_HEADER_HEIGHT = 52;
    static constexpr int RESIZE_MARGIN = 10;
    static constexpr int SIZE_GRIP_SIZE = 16;

    // Corner Radii
    static constexpr int CORNER_RADIUS_BIG = 12;
    static constexpr int CORNER_RADIUS_MEDIUM = 8;
    static constexpr int CORNER_RADIUS_SMALL = 4;

    // Specific Widget Sizes
    static constexpr int COLOR_BTN_SIZE = 32;
    static constexpr int POPUP_SHADOW_BLUR = 15;

    // Layout Constants
    static constexpr int CONTENT_MARGIN = 20;
    static constexpr int LAYOUT_SPACING = 15;
    static constexpr int FORM_VERTICAL_SPACING = 12;
    static constexpr int BTN_LAYOUT_SPACING = 10;
    
    // Widget Sizes
    static constexpr int INPUT_HEIGHT = 32;
    static constexpr int DIALOG_WIDGET_WIDTH = 400;
    static constexpr int SETTINGS_WIDGET_HEIGHT = 260;
    static constexpr int SCHEDULE_EDIT_HEIGHT = 520;
    static constexpr int PLUS_BTN_SIZE = 24;
    static constexpr int NAV_BTN_SIZE = 30;

    // DayCell / MonthWidget Constants
    static constexpr int DAYS_IN_WEEK = 7;
    static constexpr int MAX_CALENDAR_CELLS = 42;
    static constexpr int CELL_MIN_SIZE = 40;
    static constexpr int DATE_HEADER_HEIGHT = 22;

    // Font Sizes
    static constexpr int FONT_SIZE_TITLE = 11;
    static constexpr int FONT_SIZE_HEADER = 18;
    static constexpr int FONT_SIZE_FORM_LABEL = 13;
    static constexpr int FONT_SIZE_BTN = 14;
    static constexpr int FONT_SIZE_SMALL = 11;
    static constexpr int FONT_SIZE_TINY = 10;

    // Color Constants (Default/Common)
    inline const QString COLOR_BORDER_DEFAULT = "#E0E0E0";
    inline const QString COLOR_TEXT_DIM = "#777777";
    inline const QString COLOR_TEXT_LIGHT = "#999999";
    inline const QString COLOR_SUN = "#D32F2F";
    inline const QString COLOR_SAT = "#1565C0";
    inline const QString COLOR_WEEKDAY = "#757575";
}

#endif // UICONSTANTS_H
