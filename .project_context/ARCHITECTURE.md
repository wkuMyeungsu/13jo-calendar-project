# 🏗️ Architecture & Modules (Shared Context)

## 📁 File Structure (MUST be updated on every add/delete)
```plaintext
13jo-calendar-project/
├── main.cpp                  # Entry point
├── mainwindow.h / .cpp       # Main UI Controller
├── mainwindow.ui             # Main Window UI
├── CMakeLists.txt            # Build configuration
├── README.md                 # Project README
├── models/                   # Core Data & Logic Layer
│   ├── DatabaseManager.h / .cpp  # SQLite Singleton (Type-Safe)
│   ├── CalendarEngine.h / .cpp   # Layout Logic (Strict Slotting)
│   ├── Schedule.h                # Data Struct
│   ├── Category.h                # Data Struct
│   └── ScheduleSlot.h            # Layout Struct
└── widgets/                  # UI Components
    ├── UiCommon.h            # Global headers & Safe Zone Stage
    ├── UiConstants.h         # Layout Magic Numbers
    ├── StyleHelper.h         # Theme System (QSS Factory)
    ├── CustomTitleBar.h / .cpp   # Frameless custom title bar
    ├── CustomMessageBox.h / .cpp # Themed alert/confirm dialog
    ├── DayCell.h / .cpp      # Calendar Cell (Rendering only)
    ├── MonthWidget.h / .cpp  # 7x6 Calendar Grid (Orchestrator)
    ├── ColorPickerPopup.h / .cpp # Color Picker
    │
    └── (Frameless Dialog Widgets using CustomTitleBar)
        ├── ScheduleEdit.h / .cpp      # Input/Edit Form (Unified)
        ├── ScheduleManagerWidget.h / .cpp # Day List View
        ├── CategoryModifyWidget.h / .cpp # Category CRUD
        └── SettingsWidget.h / .cpp       # Preferences

├── .project_context/         # Unified Knowledge Base
├── problems/                 # Screenshots & Original Design Documents
│   ├── initial_design.pdf    # Original design draft (PDF)
│   └── result/
│       └── initial_design_draft.md # Detailed summary of original intent
├── GEMINI.md                 # Entry point for Gemini
└── CLAUDE.md                 # Entry point for Claude
```

## Module Roles
- **MainWindow**: Orchestrates all primary UI components and navigation. Handles mode switching (Normal/Mini).
- **CalendarEngine**: Pure logic module. Computes schedule slots for a given month using the 'Strict Slotting' algorithm.
- **DatabaseManager**: Handles all SQLite persistence. Returns type-safe `Schedule` and `Category` objects instead of raw maps.
- **MonthWidget**: Grid orchestrator. Receives layout results from `CalendarEngine` and distributes them to `DayCell`s.
- **DayCell**: Visual component. Renders `ScheduleSlot` items (Schedule, BarSpacer, or Empty) based on the assigned stage.
- **StyleHelper / UiConstants**: Centralizes theme-based QSS generation and layout math.
