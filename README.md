#  Qt 스마트 캘린더 프로젝트 (13조)

Qt Widgets 기반의 디자인과 카테고리 필터링 기능을 갖춘 **데스크톱 일정 관리 애플리케이션**입니다.  
사용자 환경에 맞춘 **일반/미니 모드 전환**과 **실시간 테마 시스템**을 지원합니다.

---

### 핵심 기능 (Key Features)

#### 1. 스마트 캘린더 엔진
- **부드러운 스크롤 & 애니메이션**: 마우스 휠을 이용한 부드러운 월간 이동 및 슬라이드 애니메이션.
- **동적 레이아웃 최적화**: 화면 높이에 따라 날짜별 일정 표시 개수가 자동으로 조절되는 `SafeZoneStage` 시스템.
- **카테고리 필터링**: 상단 알약(Pill) 형태의 버튼으로 특정 카테고리의 일정만 실시간으로 필터링.

#### 2. 사용자 중심 UX/UI
- **미니 모드 (Mini Mode)**: 창 너비를 줄이면 자동으로 시계와 오늘 일정을 보여주는 위젯 모드로 전환.
- **커스텀 타이틀바**: 프레임리스(Frameless) 디자인을 적용하여 윈도우 기본 틀을 벗어난 세련된 외관.
- **핀 기능 (Pin to Top)**: 미니 모드에서 창을 항상 위에 고정하여 다른 작업 중에도 일정을 확인 가능.

#### 3. 커스텀 테마 시스템
- **실시간 테마 미리보기**: 설정 창에서 테마를 선택하는 즉시 앱 전체의 색상과 스타일이 반영됨.
- **3가지 기본 테마 지원**: 
  - **Default**: 깔끔한 화이트 & 블루
  - **Soft**: 포근한 파스텔 보라
  - **Deep Blue**: 차분하고 신뢰감 있는 딥 블루

#### 4. 사용자 정의 일정 관리
- **전용 색상 팝업**: 일정 및 카테고리 설정 시 앱 디자인과 조화로운 전용 팔레트 제공.
- **시간순 자동 정렬**: 등록된 일정을 시간순(하루 종일 일정 우선)으로 자동 정렬하여 리스트업.
- **SQLite 저장**: 모든 일정과 카테고리 데이터는 로컬 DB에 안전하게 보관.

---

###  기술 스택 (Tech Stack)
- **Framework**: Qt Widgets (C++ 17)
- **Database**: SQLite
- **Build System**: CMake
- **OS**: Windows

---

###  프로젝트 구조 (Project Structure)

```plaintext
Calendar_Project/
├── models/                        # 데이터베이스 및 데이터 모델 계층
│   ├── DatabaseManager.cpp/h      # SQLite 연동 및 일정/카테고리 CRUD 로직 담당
│   └── ScheduleSlot.h             # 일정(Schedule) 및 카테고리(Category) 구조 정의
├── widgets/                       # UI 컴포넌트 및 스타일 관리 계층
│   ├── CustomTitleBar.cpp/h       # 전용 커스텀 타이틀바
│   ├── StyleHelper.h              # 테마별 색상 및 공통 CSS 중앙 관리 
│   ├── MonthWidget.cpp/h          # 월간 달력 그리드 배치 및 부드러운 스크롤 제어
│   ├── DayCell.cpp/h              # 개별 날짜 칸 UI, 일정 개수 표시 및 하이라이트
│   ├── ScheduleManagerWidget.cpp/h# 특정 날짜 일정 목록 관리 및 시간순 정렬 리스트
│   ├── ScheduleEdit.cpp/h         # 새로운 일정 입력 위젯 (ScheduleInputWidget)
│   ├── ScheduleModifyWidget.cpp/h # 기존 일정 수정 및 삭제 관리
│   ├── CategoryModifyWidget.cpp/h # 카테고리 신규 등록, 색상 변경 및 관리
│   ├── SettingsWidget.cpp/h       # 앱 환경설정
│   ├── ColorPickerPopup.cpp/h     # 색상 선택 팝업
│   ├── UiCommon.h                 # 공통 헤더 포함 및 SafeZoneStage(UI 단계) 정의
│   └── UiConstants.h              # UI 수치, 폰트 크기, 전역 상수 중앙 관리
├── main.cpp                       
├── mainwindow.cpp/h/ui            # 메인 윈도우 컨트롤러, 일반/미니 모드 전환 엔진
└── CMakeLists.txt                 # CMake 기반 빌드 시스템 구성 및 의존성 정의
```

---

###  설치 및 실행 방법

1. **사전 준비**: Qt 5.15 이상 및 CMake가 설치되어 있어야 합니다.
2. **빌드**:
   - Qt Creator에서 `CMakeLists.txt`를 엽니다.
   - 프로젝트 설정에서 Build Directory를 프로젝트 외부로 지정합니다.
   - `Build` 및 `Run` 버튼을 클릭합니다.
3. **데이터**: 앱 실행 시 실행 파일 경로에 `calendar_data.db` 파일이 자동 생성됩니다.

---

###  협업 규칙 (Team Rules)
- **Git Flow**: 작업 전 항상 `git pull`을 수행하여 충돌을 방지합니다.
- **Commit Prefix**: `feat:`, `fix:`, `docs:`, `style:`, `refactor:` 규칙을 준수합니다.
- **Style Consistency**: 공통 스타일 수정 시 반드시 `StyleHelper.h`를 통해 정의합니다.

---


