# 🛠️ 기술 스택 및 구현 가이드 (Tech Stack & Implementation Guide)

## 1. 데이터베이스 스키마 (SQLite)
*`DatabaseManager`에서 사용되는 주요 테이블 구조*

### Schedules 테이블 (일정)
- `id`: INTEGER PRIMARY KEY AUTOINCREMENT
- `categoryId`: INTEGER (외래 키)
- `title`, `content`: TEXT
- `start_date`, `end_date`: TEXT (DATETIME)
- `color`: TEXT (Hex)

### Categories 테이블 (카테고리)
- `id`: INTEGER PRIMARY KEY AUTOINCREMENT
- `name`, `color`: TEXT

## 2. 반응형 UI 시스템 (SafeZoneStage)
*창 높이에 따른 6단계 레이아웃 변경 임계값 (UiCommon.h 참조)*
- Stage 1 (< 580px) ~ Stage 6 (> 1100px)

## 3. 스타일링 및 데이터 규칙
- **StyleHelper 사용**: 하드코딩된 스타일 대신 `StyleHelper`의 함수를 사용하여 테마 일관성을 유지합니다.
- **상수 관리**: UI 수치는 `UiConstants`의 상수를 사용합니다.
- **데이터 접근**: 모든 DB 작업은 `DatabaseManager::instance()` 싱글톤을 통해서만 수행합니다.

## 4. 제약 사항 (Constraints)
- 월간 달력은 고정된 42개 셀(`MAX_CALENDAR_CELLS`) 구조로 최적화되어 있습니다.
- UI 반응형 대응은 주로 창의 '높이'를 기준으로 단계별 조절됩니다.
