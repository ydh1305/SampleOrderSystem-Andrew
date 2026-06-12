# Phase 1 검증 보고서

| 항목 | 내용 |
|------|------|
| **날짜** | 2026-06-12 |
| **대상 Phase** | Phase 1 — 인프라: 인터페이스 & JsonStorage |
| **브랜치** | `feature/phase-1-infrastructure` |
| **최종 결과** | ❌ FAIL (verify-docs FAIL 1건, verify-compliance FAIL 5건) |

---

## 1. verify-docs

**결과: FAIL** — FAIL 1건, WARN 4건 (전체 수정 완료)

### FAIL

| # | 이슈 | 위치 | 수정 여부 |
|---|------|------|-----------|
| 1 | `OrderController` 생성자 시그니처 충돌 — `PLAN.md` Phase 6(249행)은 인자 2개, `docs/api.md`는 인자 3개(`IProductionJobRepository&` 포함) | `PLAN.md` L249, `docs/api.md` | ✅ 수정 완료 |

> 수정 내용: `PLAN.md` Phase 6에 "Phase 7에서 `IProductionJobRepository&` 추가로 확장" 안내 추가

### WARN

| # | 이슈 | 위치 | 수정 여부 |
|---|------|------|-----------|
| 1 | `ProductionJob` 필드명 고립 불일치 — `SPEC.md`만 `actualProduction`/`totalProductionTime` 사용, 나머지 모든 문서는 `actualProd`/`totalTime` 사용 | `docs/SPEC.md` L65~66 | ✅ 수정 완료 |
| 2 | 생산라인 대기 수 집계 기준 충돌 — `1.main-menu.md`는 PRODUCING 상태 주문 건수 기준, `phase-4-main-menu.md` TDD 코드는 `JobStatus::WAITING` 기준 | `docs/features/1.main-menu.md`, `temp_docs/phase-11-integration.md` | ✅ 수정 완료 |
| 3 | `SPEC.md` 패키지 구조가 Java 기준으로 작성되어 있고 `ReleaseController` 누락 | `docs/SPEC.md` 섹션 8 | ✅ 수정 완료 |
| 4 | `phase-0-setup.md` 테스트 경로 오류 — `tests/` → `SampleOrderSystemTests/` | `temp_docs/phase-0-setup.md` L65 | ✅ 수정 완료 |

---

## 2. verify-test-coverage

**결과: WARN** — WARN 1건

### 빌드

- **상태**: PASS
- 환경: Debug x64, MSVC v143, 경고 0
- 비고: `test_json_parser.cpp` — C4819 경고 (코드 페이지 949) — 기능 영향 없음

### 테스트

- **상태**: PASS — **19 / 19 GREEN**
  - `JsonParserTest`: 12 / 12
  - `JsonStorageTest`: 4 / 4
  - `ConsoleUtilTest`: 3 / 3

### 커버리지

- **전체**: 78.3% (177 / 226 lines) — 기준(70%) 초과 ✅

| 파일 | 커버리지 |
|------|---------|
| `JsonParser.cpp` | 85.4% |
| `JsonStorage.cpp` | 100.0% |
| `JsonValue.cpp` | 76.4% |
| `ConsoleUtil.cpp` | **37.0%** ⚠️ |

### WARN

| # | 이슈 | 수정 여부 |
|---|------|-----------|
| 1 | `ConsoleUtil.cpp` 커버리지 37.0% — `readInt` / `readLine` / `resetColor` 미테스트 | ❌ 미수정 |

---

## 3. verify-compliance

**결과: FAIL** — FAIL 5건

### FAIL

| # | 이슈 | 파일 | 수정 여부 |
|---|------|------|-----------|
| 1 | `IModel.h` 설계 초과 구현 — 설계는 순수 마커 인터페이스(`virtual ~IModel()` 만), 실제는 `toJson() const = 0` · `fromJson(const JsonValue&) = 0` 순수 가상 함수 2개 무단 추가 | `SampleOrderSystem/include/IModel.h` | ❌ 미수정 |
| 2 | `ConsoleUtil::resetColor()` 누락 — 설계 문서에 명시된 함수이나 헤더/구현 모두 미선언·미구현 | `SampleOrderSystem/src/view/ConsoleUtil.h` | ❌ 미수정 |
| 3 | `ConsoleUtil::readInt` / `readLine` 시그니처 불일치 — 설계: `readInt(const std::string& prompt)` / `readLine(const std::string& prompt)`, 실제: 매개변수 제거됨 | `SampleOrderSystem/src/view/ConsoleUtil.h` | ❌ 미수정 |
| 4 | `ConsoleUtil` 선언 방식 불일치 — 설계: `namespace ConsoleUtil { ... }`, 실제: `class ConsoleUtil { public: static ... }` (기능상 동등하나 명세 불일치) | `SampleOrderSystem/src/view/ConsoleUtil.h` | ❌ 미수정 |
| 5 | Phase 0 `data/` `.gitignore` 포함 여부 미확인 (파일 존재로만 검증, 내용 미검증) | `.gitignore` | ❌ 미수정 |

---

## 4. verify-tdd

**결과: WARN** — WARN 2건

### RED 단계

- **상태**: PASS — 3개 모듈 모두 테스트 파일 선작성 Git 커밋 순서 검증 완료

| 모듈 | 커밋 | 비고 |
|------|------|------|
| JsonParser | `87d3bca` | 구현 없이 테스트+헤더만 커밋 (빌드 실패 명시) |
| JsonStorage | `421e20f` | 구현 없이 테스트+헤더만 커밋 (빌드 실패 명시) |
| ConsoleUtil | `e76d7d4` | 빈 스텁 방식으로 테스트 RED 확인 후 구현 |

### GREEN 단계

- **상태**: PASS — gmock 패턴 올바르게 사용, 임시 파일 격리(`TEST_F` + `SetUp`/`TearDown`)

### REFACTOR 단계

- **상태**: PASS — 3개 모듈 모두 REFACTOR 커밋 존재, 중복 제거 결과 반영

| 모듈 | 커밋 |
|------|------|
| JsonParser | `bde466c` |
| JsonStorage | `3e30693` |
| ConsoleUtil | `edc7e82` |

### 테스트 커버율

| 모듈 | 설계 케이스 | 실제 케이스 |
|------|-------------|-------------|
| JsonParser | 11 | 12 (109%) |
| JsonStorage | 4 | 4 (100%) |

### WARN

| # | 이슈 | 수정 여부 |
|---|------|-----------|
| 1 | `ConsoleUtil::readInt()` / `readLine()` — `prompt` 매개변수 제거된 채 구현, 해당 시그니처 테스트 없음 | ❌ 미수정 |
| 2 | `ConsoleUtil::resetColor()` — 설계 명세에 있으나 미구현, 테스트 없음 | ❌ 미수정 |

---

## 다음 단계 권장 액션

아래 항목은 Phase 2 진입 전 처리를 권장합니다.

### 즉시 수정 필요 (FAIL)

1. **`IModel.h` 설계 초과 구현 제거**
   - `toJson()` / `fromJson()` 순수 가상 함수 삭제
   - 설계대로 `virtual ~IModel() = default;` 만 남길 것
   - 파일: `SampleOrderSystem/include/IModel.h`

2. **`ConsoleUtil` 시그니처 및 선언 방식 정합**
   - `class ConsoleUtil` → `namespace ConsoleUtil` 으로 변경
   - `readInt()` → `readInt(const std::string& prompt)` 로 매개변수 복원
   - `readLine()` → `readLine(const std::string& prompt)` 로 매개변수 복원
   - `resetColor()` 함수 선언 및 구현 추가
   - 파일: `SampleOrderSystem/src/view/ConsoleUtil.h`, `ConsoleUtil.cpp`

3. **`ConsoleUtil` 누락 함수 테스트 추가 (TDD)**
   - `readInt(prompt)` / `readLine(prompt)` / `resetColor()` 에 대한 단위 테스트 작성
   - RED → GREEN → REFACTOR 사이클 준수

4. **`.gitignore` `data/` 항목 포함 여부 명시적 확인**

### 권장 수정 (WARN)

5. **`ConsoleUtil.cpp` 커버리지 개선** (현재 37.0%)
   - `readInt` / `readLine` / `resetColor` 테스트 추가 시 자연히 해소됨
