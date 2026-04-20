# 13jo-calendar-project 
📅 **Qt 일정관리 프로젝트 (Calendar_Project)** Qt Widgets 기반의 카테고리 필터링 기능을 갖춘 월간 일정 관리 애플리케이션입니다.

---

### 🛠 기술 스택
- **Framework**: Qt Widgets (C++ 17)
- **Database**: SQLite (QtSql)
- **IDE**: Qt Creator

---

### 📂 프로젝트 구조
설계서의 모듈화 원칙에 따라 다음과 같이 관리합니다.

```plaintext
Calendar_Project/
├── main.cpp                  # 진입점
├── mainwindow.h / .cpp       # 메인 윈도우 (전체 관리)
├── models/                   # 데이터 및 비즈니스 로직
│   ├── ScheduleItem.h        # 일정 데이터 모델
│   └── DatabaseManager.h     # DB 싱글톤 관리
├── widgets/                  # 커스텀 UI 부품
│   ├── DayCell.h             # 날짜 개별 셀 (호버 애니메이션 포함)
│   ├── CategoryFilterBar.h   # 상단 카테고리 필터
│   └── ScheduleEditDialog.h  # 일정 편집 창
└── .gitignore                # 빌드 아티팩트 제외 설정
```

---

### ⚠️ 개발 및 협업 규칙 (필독)

#### 1. Git 관리 대상 제외 (Ignore)
다음 파일들은 로컬 환경에 의존하므로 절대 저장소에 올리지 않습니다.
- **build/** 폴더 (모든 빌드 아티팩트)
- **\*.user** 파일 (Qt Creator 개인 설정)
- **.qtcreator/** 폴더 및 **.idx** 캐시 파일

#### 2. 빌드 환경 설정
Qt Creator에서 프로젝트 오픈 시 **Build Directory**를 프로젝트 폴더 외부로 설정하는 것을 권장합니다.  
> 예: `../build-Calendar_Project-Desktop-Debug`

#### 3. 작업 흐름 (Workflow)
- **Pull 먼저**: 작업 시작 전 반드시 `git pull origin main`을 수행합니다.
- **Conflict 방지**: `MainWindow`나 `.pro` 파일 수정 시 팀원과 미리 공유합니다.
- **Commit 메시지**: `feat:`, `fix:`, `chore:`, `docs:` 등의 접두사를 사용합니다.

---

### 🚀 주요 핵심 기능
- **DayCell Hover UX**: 일정 없는 셀 호버 시 (+) 버튼 페이드인 애니메이션.
- **Category Filter**: 상단 토글 버튼을 통한 실시간 일정 필터링.
- **SQLite Integration**: 가벼운 로컬 DB를 활용한 일정 영구 저장.