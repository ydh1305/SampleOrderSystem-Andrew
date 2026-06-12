# Phase 5 검증 결과

- **날짜**: 2026-06-12
- **Phase**: Phase 5 — View 레이어 6종 (브랜치명: feature/phase-5-view)
- **브랜치**: `feature/phase-5-view`
- **최종 결과**: FAIL

> ⚠️ **브랜치/구현 범위 불일치**: `feature/phase-5-view`는 View 레이어 6종만 구현. PLAN.md 기준 Phase 5의 핵심 산출물인 SampleController가 미구현 상태. Controller·MockRepositories.h는 `feature/phase-6-controller` 브랜치에 구현됨.

---

## verify-docs: FAIL (FAIL 2건, WARN 3건)

### FAIL 2건

| # | 항목 |
|---|------|
| 1 | `docs/features/6.production-line.md`: `"std::queue 기반"` ↔ `architecture.md` `"파일 기반 영속 큐"` 충돌 (누적 미수정) |
| 2 | `temp_docs/phase-6-order-placement.md` OrderController 2인자 ↔ `docs/api.md` 3인자 불일치 |

### WARN 3건

| # | 항목 |
|---|------|
| 1 | `docs/features/2,3,4.*.md`: `class` ↔ PLAN.md `struct` 불일치 |
| 2 | `docs/SPEC.md` Order/ProductionJob `LocalDateTime` ↔ C++ `std::string` 타입 표기 불일치 |
| 3 | `temp_docs/phase-5-sample-management.md` SampleView `getInput()` override 처리 방침 미명시 |

---

## verify-test-coverage: WARN (95/95 PASS)

- **빌드**: PASS (Debug x64)
- **테스트**: PASS — 95 / 95 GREEN
- **커버리지**: 79.7% (기준 70% 초과)

| 테스트 모듈 | 결과 |
|-------------|------|
| JsonParserTest | 12/12 |
| JsonStorageTest | 4/4 |
| ConsoleUtilTest | 3/3 |
| OrderStatusTest | 3/3 |
| SampleModelTest | 4/4 |
| OrderModelTest | 4/4 |
| ProductionJobModelTest | 3/3 |
| SampleRepositoryTest | 6/6 |
| ProductionCalculatorTest | 8/8 |
| OrderApprovalServiceTest | 5/5 |
| ProductionServiceTest | 4/4 |
| OrderRepositoryTest | 7/7 |
| ProductionJobRepositoryTest | 8/8 |
| SampleViewTest | 6/6 |
| OrderViewTest | 5/5 |
| MonitoringViewTest | 3/3 |
| ReleaseViewTest | 3/3 |
| ProductionViewTest | 4/4 |
| MainViewTest | 3/3 |

### WARN 1건

| # | 항목 |
|---|------|
| 1 | View 레이어 커버리지 낮음 — ConsoleUtil.cpp 37.0%, SampleView.cpp 39.7%, OrderView.cpp 43.3%, ReleaseView.cpp 48.3%, ProductionView.cpp 60.0% |

---

## verify-compliance: FAIL (12건)

### FAIL 12건

| # | 항목 |
|---|------|
| 1 | `SampleController.h/.cpp` 미존재 (src/controller/ 디렉터리 없음) |
| 2 | `SampleController(ISampleRepository&)` 생성자 미구현 |
| 3 | `registerSample()` 유효성 검사 로직 미구현 |
| 4 | `listSamples()` 미구현 |
| 5 | `searchSamples()` 미구현 |
| 6 | `test_sample_controller.cpp` 미존재 (T1~T12 전체 누락) |
| 7 | `MockRepositories.h` 미존재 (mocks/ 디렉터리 없음) |
| 8 | `SampleView` — `IView` 상속 없음 |
| 9 | SampleView 메서드명 불일치 (`printSubMenu` → `showSubMenu`, `getRegisterInput` → `read*` 개별 분리) |
| 10 | `RegisterInput` 구조체 미구현 |
| 11 | `showSaveSuccess` 시그니처 불일치 (`Sample&` 요구 → `string id` 구현) |
| 12 | 목록 테이블 컬럼 정렬 미충족 |

---

## verify-tdd: WARN (2건)

### WARN 2건

| # | 항목 | 위치 |
|---|------|------|
| 1 | `OrderController::placeOrder()` 47~49행 ORD 포맷 조합 로직 인라인 중복 — `OrderIdGenerator::generate()` 미사용 | `OrderController.cpp:47-49` |
| 2 | `SampleController::listSamples()` 빈 목록 경계값 테스트 누락 | `test_controllers.cpp` |

### 부분 PASS 항목

| 항목 | 결과 |
|------|------|
| View 레이어 6종 RED → GREEN 커밋 순서 준수 | ✅ |
| Controller 레이어 RED → GREEN 커밋 순서 준수 | ✅ |
| MockRepositories.h MOCK_METHOD 패턴 | ✅ |
| EXPECT_CALL 34건 Mock 상호작용 검증 | ✅ |

---

## 다음 단계 권장 액션

1. `SampleController.h/.cpp` 구현 (ISampleRepository& 의존, 유효성 검사)
2. `MockRepositories.h` 생성 + `test_sample_controller.cpp` T1~T12 작성
3. `SampleView` — `IView` 상속 추가 및 메서드명 설계 문서와 정렬
4. `OrderController::placeOrder()` → `OrderIdGenerator::generate()` 위임으로 교체
5. `docs/features/6.production-line.md` 133행 표현 수정
