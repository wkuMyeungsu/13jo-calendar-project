# ?? Qt 일정관리 프로젝트 (Calendar_Project) 
 
Qt Widgets 기반의 카테고리 필터링 기능을 갖춘 월간 일정 관리 애플리케이션입니다. 
 
## ?? 기술 스택 
- **Framework**: Qt Widgets (C++ 17) 
- **Database**: SQLite (QtSql) 
- **IDE**: Qt Creator 
 
## ?? 프로젝트 구조 
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
 
## ?? 개발 및 협업 규칙 
1. **Git 관리 대상 제외**: build/ 폴더, *.user 파일 등은 커밋하지 않습니다. 
2. **빌드 설정**: Build Directory를 프로젝트 폴더 외부로 설정을 권장합니다. 
3. **Workflow**: 작업 전 `pull` 필수, 커밋 컨벤션(`feat:`, `fix:`) 준수. 
 
## ?? 주요 핵심 기능 
- **DayCell Hover UX**: 일정 없는 셀 호버 시 (+) 버튼 애니메이션. 
- **Category Filter**: 상단 토글 버튼을 통한 실시간 필터링. 
- **SQLite Integration**: 로컬 DB를 활용한 데이터 영구 저장. 
