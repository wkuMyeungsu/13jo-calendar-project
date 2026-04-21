# 📂 Project Structure: 13jo-calendar-project

이 프로젝트는 Qt Widgets 기반의 캘린더 애플리케이션으로, **MVC(Model-View-Controller)** 패턴의 변형된 구조를 따르며 기능별로 모듈화되어 있습니다.

---

## 🏗️ 전체 구조 개요

```plaintext
13jo-calendar-project/
├── main.cpp                  # 애플리케이션 진입점
├── mainwindow.h / .cpp       # 메인 윈도우 (전체 레이아웃 및 위젯 관리)
├── mainwindow.ui             # 메인 윈도우 UI 디자인 (Qt Designer)
├── CMakeLists.txt            # 빌드 설정 파일
├── models/                   # 데이터 관리 및 비즈니스 로직 (Model)
└── widgets/                  # 커스텀 UI 컴포넌트 및 스타일 (View)
```

---

## 📦 세부 모듈 설명

### 1. Root Directory
- **`main.cpp`**: `QApplication` 인스턴스를 생성하고 메인 윈도우를 실행합니다.
- **`mainwindow.h / .cpp / .ui`**: 상단 바, 사이드바(설정), 중앙 캘린더 영역을 통합 관리하는 최상위 위젯입니다.

### 2. `models/` (Data & Logic)
데이터의 저장, 가공, 배치를 담당합니다.
- **`DatabaseManager.h / .cpp`**: SQLite 데이터베이스 연결 및 일정/카테고리의 CRUD(생성, 읽기, 수정, 삭제)를 처리하는 싱글톤 클래스입니다.
- **`CalendarEngine.h / .cpp`**: 일정을 캘린더의 한정된 슬롯에 효율적으로 배치하기 위한 레이아웃 계산 알고리즘을 담당합니다.
- **`Schedule.h`**: 일정 데이터를 담는 구조체 (`id`, `title`, `start`, `end`, `color` 등).
- **`Category.h`**: 일정 분류를 위한 카테고리 구조체.
- **`ScheduleSlot.h`**: 엔진에서 계산된 일정의 위치 정보를 담는 헬퍼 구조체.

### 3. `widgets/` (UI Components)
사용자 인터페이스와 스타일링을 담당합니다.
- **`MonthWidget.h / .cpp`**: 7x6 그리드 형태의 월간 달력 본체입니다.
- **`DayCell.h / .cpp`**: 날짜 하나를 나타내는 개별 셀입니다. 일정 표시 및 호버 시 (+) 버튼 노출 기능을 포함합니다.
- **`ScheduleEdit.h / .cpp`**: **(통합 위젯)** `ScheduleEditDialog` 클래스를 포함하며, 신규 일정 추가와 기존 일정 수정을 모두 담당합니다.
- **`ScheduleManagerWidget.h / .cpp`**: 특정 날짜의 일정 목록을 보여주고 관리하는 위젯입니다. 항목 클릭 시 `ScheduleEditDialog`를 호출합니다.
- **`CategoryModifyWidget.h / .cpp`**: 카테고리 추가/삭제 및 색상 변경을 위한 설정 위젯입니다.
- **`SettingsWidget.h / .cpp`**: 테마 변경 등 애플리케이션 설정을 관리합니다.
- **`CustomTitleBar.h / .cpp`**: 기본 윈도우 타이틀바 대신 사용되는 커스텀 디자인 타이틀바입니다.
- **`StyleHelper.h`**: QSS(Qt Style Sheets)를 동적으로 생성하여 테마(`Default`, `Soft`, `DeepBlue`)를 적용합니다.
- **`UiConstants.h`**: 폰트 크기, 여백, 고정 색상 등 UI 전역 상수를 정의합니다.
- **`UiCommon.h`**: 공통 헤더 포함 및 윈도우 크기에 따른 반응형 레이아웃 단계(`SafeZoneStage`)를 정의합니다.

---

## 🛠️ 주요 설계 원칙
1.  **관심사 분리**: UI 위젯(`widgets/`)은 데이터 처리를 직접 하지 않고 `DatabaseManager`나 `CalendarEngine`을 호출합니다.
2.  **반응형 레이아웃**: `UiCommon.h`의 `getStageForHeight` 함수를 통해 창 높이에 따라 일정 슬롯 개수와 폰트 크기가 자동으로 조절됩니다.
3.  **동적 스타일링**: 모든 UI 요소는 `StyleHelper`를 거쳐 스타일을 적용받으므로, 코드 수정 없이 테마 전환이 가능합니다.
4.  **UI 모듈 통합**: 유사한 기능을 가진 `ScheduleEdit`과 `ScheduleModifyWidget`을 `ScheduleEditDialog`로 통합하여 유지보수성을 높였습니다.
