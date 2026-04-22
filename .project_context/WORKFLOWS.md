# 🔄 Core Workflows (Logic Flow)

## 1. 일정 데이터 로드 및 레이아웃 (Layout Flow)
1. **DB Fetch**: `MainWindow`가 `DatabaseManager`를 통해 현재 월의 `QList<Schedule>`을 가져옴.
2. **Logic Engine**: `CalendarEngine::computeLayout(year, month, schedules)` 호출.
   - **Sorting**: 시작일 순, 긴 기간 순으로 일정 정렬.
   - **Slotting**: 'Strict Slotting' 알고리즘을 사용하여 각 일정에 수직 슬롯(0, 1, 2...) 배정.
   - **BarSpacer**: 멀티데이 일정이 끝나는 지점과 이어지는 지점 사이의 빈 공간에 `BarSpacer` 삽입 (연속성 유지).
3. **Distribution**: `MonthWidget::updateMonth()`가 계산된 `QMap<QDate, QList<ScheduleSlot>>`을 각 `DayCell`에 전달.
4. **Rendering**: `DayCell`이 `setSchedules()`를 호출받아 실제 `QLabel` 바(bar) 생성.

## 2. 윈도우 모드 전환 (Mode Switch)
- **Normal Mode**: 월간 캘린더 전체 표시.
- **Mini Mode**: 
  - `MonthWidget` 축소 및 `DayCell`을 Stage 1(최소 슬롯)로 강제 조정.
  - 우측 패널에 오늘 일정을 카드 형태로 리스트업.

## 3. 일정 CRUD 워크플로우
- **Create**: 빈 날짜 더블 클릭 → `ScheduleEdit` 팝업 → `DatabaseManager::addSchedule()` → `MainWindow::updateCalendar()`.
- **Edit/Delete**: 일정 바 클릭 → `ScheduleManagerWidget` → 일정 선택 → `ScheduleModifyWidget` → DB 업데이트 → UI 리프레시.

## 4. 테마 및 스타일 적용
- 모든 위젯 생성 시 `StyleHelper`를 통해 QSS 문자열 획득 및 `setStyleSheet()` 호출.
- 색상 선택 시 `ColorPickerPopup`을 사용하여 프리셋 기반 컬러 선택 제공.
