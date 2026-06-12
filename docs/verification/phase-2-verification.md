# Phase 2 검증 결과 보고서

| 항목 | 내용 |
|------|------|
| 날짜 | 2026-06-12 |
| Phase | Phase 2 — 도메인 모델 |
| 브랜치 | feature/phase-2-domain-models |
| 최종 결과 | ❌ **FAIL** |

**실패 원인 요약**: verify-docs FAIL (3건), verify-compliance FAIL (1건)

---

## 1. verify-docs — ❌ FAIL (FAIL 3건, WARN 3건)

### FAIL 1 — 주문번호 순번 정책 충돌 ❌ 미수정

- **충돌 위치**
  - SPEC.md 5절 + `docs/features/3.order-placement.md`: "시스템 전체 단조 증가, 재시작 후에도 유지"
  - `docs/database.md`, `docs/api.md`, `temp_docs/phase-6-order-placement.md` TDD: "날짜별 독립 순번, 날짜 바뀌면 0001 리셋"
- **기준**: `docs/database.md` 방식(날짜별 독립 순번)을 구현 기준으로 채택
- **필요 조치**: SPEC.md 5절, `docs/features/3.order-placement.md` 내용을 database.md 방식으로 수정

### FAIL 2 — phase-4-main-menu.md 내부 자기 모순 (생산라인 대기 수) ❌ 미수정

- **충돌 위치**
  - 구조체 주석 + TDD T8 표: "PRODUCING 상태 수" 기준으로 기술
  - 실제 Mock 코드: `findByStatus(JobStatus::WAITING)` 사용
- **필요 조치**: 구조체 주석과 TDD T8 표를 WAITING 기준으로 통일

### FAIL 3 — docs/features/4.order-approval.md ProductionJob 필드 누락 ❌ 미수정

- **문제**: `startedAt`, `completedAt` 필드가 코드 예시에서 누락됨
- **불일치 대상**: SPEC.md, `docs/database.md` 등 다른 모든 문서에는 해당 필드 포함
- **필요 조치**: `4.order-approval.md` 코드 예시에 `startedAt`, `completedAt` 필드 추가

### WARN 1 — docs/features/6.production-line.md 큐 구현 표현 불일치 ❌ 미수정

- `6.production-line.md`: "ProductionQueue — std::queue 기반"
- `docs/architecture.md` 5.2절: "IProductionJobRepository 기반 (파일 영속 큐)"
- **권장 조치**: `6.production-line.md` 표현을 architecture.md 기준으로 정렬

### WARN 2 — docs/SPEC.md 6절 저장 방식 미확정 표현 ❌ 미수정

- SPEC.md 6절: "JSON / SQLite / DB 중 선택" — 이미 JSON으로 확정된 상태
- CLAUDE.md, `docs/database.md`, `docs/architecture.md` 모두 JSON으로 고정
- **권장 조치**: SPEC.md 6절을 "JSON 파일"로 명시 수정

### WARN 3 — docs/api.md Phase 6 전략 주석 미반영 ❌ 미수정

- PLAN.md에는 "OrderController 2개 인자" 과도 시그니처 전략 주석이 명시됨
- `docs/api.md`에는 해당 내용 미반영
- **권장 조치**: `api.md`에 Phase 6 시그니처 전략 주석 추가

---

## 2. verify-test-coverage — ✅ PASS (WARN 1건)

- **빌드**: ✅ PASS — Debug x64, MSVC v143, 오류 0
- **테스트**: ✅ PASS — **30 / 30 GREEN**

| 테스트 클래스 | 결과 |
|--------------|------|
| JsonParserTest | 12 / 12 ✅ |
| JsonStorageTest | 4 / 4 ✅ |
| ConsoleUtilTest | 3 / 3 ✅ |
| OrderStatusTest | 3 / 3 ✅ |
| SampleModelTest | 3 / 3 ✅ |
| OrderModelTest | 3 / 3 ✅ |
| ProductionJobModelTest | 2 / 2 ✅ |

- **커버리지**: ✅ **84.8%** (273 / 322 lines) — 기준(70%) 초과

| 파일 | 커버리지 |
|------|---------|
| model/Sample.cpp | 100.0% |
| model/Order.cpp | 100.0% |
| model/ProductionJob.cpp | 100.0% |
| model/OrderStatus.h | 93.8% |
| model/JobStatus.h | 75.0% |
| storage/JsonParser.cpp | 85.4% |
| storage/JsonStorage.cpp | 100.0% |
| storage/JsonValue.cpp | 81.9% |
| view/ConsoleUtil.cpp | **37.0%** ⚠️ |

### WARN 1 — ConsoleUtil.cpp 커버리지 37.0% ❌ 미수정

- Phase 1 이월 항목으로, Phase 3 이후 해소 예정

---

## 3. verify-compliance — ❌ FAIL (FAIL 1건, WARN 1건)

### FAIL 1 — fromJson 누락 필드 기본값 처리 없음 ❌ 미수정

- **영향 파일**
  - `SampleOrderSystem/src/model/Sample.cpp`
  - `SampleOrderSystem/src/model/Order.cpp`
  - `SampleOrderSystem/src/model/ProductionJob.cpp`
- **문제**: `Sample::fromJson`, `Order::fromJson`, `ProductionJob::fromJson` 세 곳 모두 `json["key"]` 직접 참조 방식 사용 → 키 없으면 `std::out_of_range` 예외 발생
- **설계 요구사항**: `contains(key)` 확인 후 기본값 처리
- **필요 조치**: 세 파일의 `fromJson` 구현에 `contains(key)` 가드 추가

### WARN 1 — REFACTOR 검토 보고 누락 ❌ 미수정

- "공통 날짜 필드 처리 중복 제거 여부"에 대한 명시적 검토 기록 없음
- 런타임 영향 없음
- **권장 조치**: 이후 Phase부터 REFACTOR 검토 결과를 커밋 메시지 또는 PR 설명에 명시

---

## 4. verify-tdd — ✅ PASS (WARN 3건)

- **RED 단계**: ✅ PASS — 4개 모듈 모두 테스트 파일 선작성, git 커밋 순서 검증 완료

| 커밋 | 내용 |
|------|------|
| cdf95e6 | OrderStatus / JobStatus (RED) |
| 10266a2 | Sample (RED) |
| 9a7950b | Order (RED) |
| 3992574 | ProductionJob (RED) |

- **GREEN 단계**: ✅ PASS — 설계 T1~T11 테스트 케이스 100% 구현, 순수 모델에 `TEST()` 적절히 사용
- **REFACTOR 단계**: ✅ PASS — 4개 모듈 모두 REFACTOR 커밋 존재

| 커밋 | 내용 |
|------|------|
| 5adfce5 | OrderStatus / JobStatus (네이밍 통일, IModel::fromJson 가상 메서드 제거) |
| 13de569 | Sample (include 위임 구조 적용) |
| dadc600 | Order |
| 17d249f | ProductionJob |

### WARN 1 — GREEN 커밋 별도 분리 없음 ❌ 미수정

- `[refactor]` 커밋이 최초 구현(GREEN)과 정리(REFACTOR)를 동시에 포함
- RED → (GREEN+REFACTOR 병합) 2단계 구조로 진행됨
- **권장 조치**: 이후 Phase부터 GREEN 커밋을 별도 분리하여 3단계 이력 유지

### WARN 2 — JobStatus::RUNNING, DONE 직접 검증 테스트 없음 ❌ 미수정

- T11 라운드트립에서 `WAITING` 하나만 간접 검증
- **권장 조치**: `RUNNING`, `DONE` 상태 직접 검증 테스트 케이스 추가

### WARN 3 — OrderStatus 사전 구현 흔적 ❌ 미수정

- 커밋 `6ae1aaa` (Phase 1)에서 `include/OrderStatus.h`에 `orderStatusToString()` 선행 구현
- 함수명이 RED 테스트의 `toString()`과 달라 실질 RED 상태는 유지됐으나 일관성 저하
- **권장 조치**: 이후 Phase에서 선행 구현 방지, RED 단계 이전 잔존 코드 점검

---

## 다음 단계 권장 액션

### 즉시 수정 필요 (블로커)

1. **[compliance] fromJson 기본값 처리 누락**
   - 파일: `model/Sample.cpp`, `model/Order.cpp`, `model/ProductionJob.cpp`
   - `json["key"]` → `contains(key)` 확인 후 기본값 반환 패턴으로 수정

2. **[docs] 주문번호 순번 정책 충돌 해소**
   - `docs/SPEC.md` 5절, `docs/features/3.order-placement.md`를 날짜별 독립 순번(database.md 기준)으로 수정

3. **[docs] phase-4-main-menu.md 생산라인 대기 수 기준 통일**
   - 구조체 주석 + TDD T8 표를 `JobStatus::WAITING` 기준으로 수정

4. **[docs] docs/features/4.order-approval.md ProductionJob 필드 보완**
   - `startedAt`, `completedAt` 필드를 코드 예시에 추가

### 권장 수정 (품질 개선)

5. **[docs] SPEC.md 6절 저장 방식 확정 표현으로 수정** — "JSON 파일"로 명시
6. **[docs] docs/features/6.production-line.md 큐 표현 정렬** — architecture.md 기준으로 통일
7. **[docs] docs/api.md Phase 6 전략 주석 추가** — PLAN.md 내용 반영
8. **[tdd] 이후 Phase부터 GREEN 커밋 별도 분리** — 3단계 이력 유지
9. **[tdd] JobStatus::RUNNING, DONE 직접 검증 테스트 추가** — Phase 3 착수 전 보완
10. **[coverage] ConsoleUtil.cpp 커버리지 개선** — Phase 3 이후 해소 예정
