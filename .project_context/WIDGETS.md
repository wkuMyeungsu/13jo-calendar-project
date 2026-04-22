# 🧩 Widget Components

## Calendar Views
- **MonthWidget**: Manages 42 DayCells. Handles single-cell selection logic to ensure only one cell is highlighted at a time.
- **DayCell**:
    - **Selection**: Highlights with a 2px primary-colored border when clicked.
    - **Today**: Highlights with a 2px orange (#FF9800) border.
    - **Hover Hint**: Shows a centered '+' symbol on empty cells using `m_hoverHintLabel`.
    - **Style Sync**: Uses `updateStyle()` as the central logic to synchronize fonts (date & overflow label) and borders, ensuring consistent rendering even when stages or selection states change.
    - **Interaction**: Double-clicking an empty cell triggers schedule creation; double-clicking a scheduled cell opens the manager.

## Management Widgets (Frameless Dialogs)
All management widgets are implemented as frameless dialogs that use the `CustomTitleBar`. They are created with a fixed size and have resizing disabled.
- **ScheduleInputWidget**: Circular color picker and unified padding/spacing.
- **ScheduleManagerWidget**: Day-view list with standardized item styling.
- **ScheduleModifyWidget**: Form for updating/deleting entries.
- **CategoryModifyWidget**: CRUD for categories with circular color buttons.
- **SettingsWidget**: Application settings and theme selection.

## Core UI & Utilities
- **CustomTitleBar**: Provides the custom window frame and controls (drag, min, max, close).
    - It is used by `MainWindow` (with resizing enabled) and all frameless dialogs.
    - Features a `setResizable(bool)` method to enable/disable the top-right corner resize handle, ensuring dialogs have a fixed size.
- **UiConstants.h**: Global truth for dimensions (radii, margins, spacing).
- **StyleHelper.h**: Centralized QSS generation. Includes `getDayCellHoverBg()` for theme-aware interaction.
