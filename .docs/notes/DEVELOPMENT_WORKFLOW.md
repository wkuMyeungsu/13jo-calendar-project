# 🚀 개발 워크플로우 (Development Workflow)

## 1. 빌드 및 실행 (Build & Run)
### 필수 요구 사항
- Qt 5.15+ 또는 Qt 6.x (Widgets, Sql 모듈)
- CMake 3.16+
- C++17 지원 컴파일러

### 빌드 명령어
```bash
mkdir build
cd build
cmake ..
cmake --build .
```
*Qt Creator 사용 시: 빌드 디렉터리를 프로젝트 폴더 외부로 설정하는 것을 권장합니다.*

## 2. Git 협업 규칙 (Git Workflow)
- **무시 대상**: `build/`, `*.user`, `.qtcreator/` 등 로컬 설정 파일은 커밋하지 않습니다.
- **커밋 메시지 컨벤션**: 다음 접두사를 사용합니다.
  - `feat:` 새로운 기능 추가
  - `fix:` 버그 수정
  - `docs:` 문서 수정
  - `chore:` 빌드 업무, 패키지 매니저 설정 등
- **Pull First**: 작업 시작 전 반드시 최신 코드를 pull 받습니다.

## 3. 테스트 전략 (Testing Strategy)
- 현재 프로젝트는 수동 UI 테스트를 기본으로 합니다.
- 기능 추가 시 다양한 창 크기에서 `SafeZoneStage`가 정상 작동하는지 반드시 확인해야 합니다.
