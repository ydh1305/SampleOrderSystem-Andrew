# 프로젝트 구현 계획 — 반도체 시료 생산주문관리 시스템

> 검토 후 각 Phase 세부 계획을 확정한다.  
> 모든 기능 구현은 TDD (Red → Green → Refactor) 사이클로 진행한다.
>
> **테스트 프레임워크**: gmock 1.11.0 (NuGet 설치 완료)  
> **테스트 프로젝트**: `SampleOrderSystemTests/` — 모든 테스트 파일은 이 프로젝트 내에 위치  
> **Mock 전략**: Controller 단위 테스트 시 Repository를 Mock으로 교체 (의존성 주입)

---

## 전체 Phase 구성 요약

| Phase | 이름 | 핵심 산출물 | 선행 조건 |
|-------|------|------------|----------|
| 0 | 프로젝트 셋업 | VS 솔루션, 디렉터리 구조, main.cpp, gmock 설정 | — |
| 1 | 인프라 — 인터페이스 & JSON 스토리지 | IModel/IView/IController, JsonStorage | Phase 0 |
| 2 | 도메인 모델 | Sample, Order, ProductionJob, Enum | Phase 0 |
| 3 | Repository 계층 | **ISampleRepo/IOrderRepo/IProductionJobRepo** (인터페이스) + 구체 구현 | Phase 1, 2 |
| 4 | 메인 메뉴 | MainController, MainView, **MockRepositories.h** | Phase 1, 2, 3 |
| 5 | 시료 관리 | SampleController, SampleView | Phase 3, 4 |
| 6 | 시료 주문 접수 | OrderController(접수), OrderView(접수) | Phase 3, 4 |
| 7 | 주문 승인/거절 | OrderController(승인), ProductionQueue | Phase 3, 4, 6 |
| 8 | 모니터링 | MonitoringController, MonitoringView | Phase 3, 4 |
| 9 | 생산라인 | ProductionController, ProductionView | Phase 3, 4, 7 |
| 10 | 출고 처리 | ReleaseController, ReleaseView | Phase 3, 4, 7 |
| 11 | 통합 & 마무리 | 통합 테스트, UI 컬러, 더미 데이터 | Phase 5~10 |

---

## Phase 0 — 프로젝트 셋업

### 목표
Visual Studio 2022 솔루션 생성 및 빌드 가능한 최소 골격 구축

### 작업 항목
- [x] Visual Studio 2022 솔루션 파일 및 C++17 프로젝트 생성
- [ ] 디렉터리 구조 생성
  ```
  SampleOrderSystem/
  ├── src/           (model/, view/, controller/, repository/, storage/)
  ├── include/       (공용 인터페이스 헤더)
  └── main.cpp
  SampleOrderSystemTests/   ← gmock 테스트 프로젝트
  └── main_test.cpp         ← RUN_ALL_TESTS() 진입점
  data/                     ← 런타임 JSON (gitignore)
  ```
- [x] `main.cpp` 진입점 (Hello World 수준)
- [ ] `SampleOrderSystemTests/main_test.cpp` — gmock RUN_ALL_TESTS() 진입점
- [ ] `.gitignore`에 `data/`, `*.exe`, `x64/`, `packages/` 추가
- [ ] 빌드 성공 확인 (경고 0)

### gmock 테스트 설정
```cpp
// SampleOrderSystemTests/main_test.cpp
#include <gmock/gmock.h>
int main(int argc, char** argv) {
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
```
- **자체 매크로(TestRunner.h, ASSERT_EQ) 사용 금지** → gmock/gtest만 사용

### 검증
- `main.cpp` 빌드 & 실행 → 콘솔에 기동 메시지 출력
- `SampleOrderSystemTests` 빌드 → 0 tests run (오류 없음)

---

## Phase 1 — 인프라: 인터페이스 & JsonStorage

### 목표
모든 계층이 공유하는 인터페이스와 JSON 파일 I/O 기반 스토리지 구현

### 작업 항목

#### 1-A. 인터페이스 정의
- [ ] `IModel` — 공통 모델 인터페이스 (필요 최소)
- [ ] `IView` — `render()`, `getInput()` 추상화
- [ ] `IController` — `run()` 추상화

#### 1-B. JsonStorage (PoC: DataPersistence-Andrew 참조)
- [ ] `JsonValue` — null / bool / int / double / string / array / object 지원
- [ ] `JsonParser` — 재귀 하강 파서 (외부 라이브러리 없음)
- [ ] `JsonStorage` — 파일 Read/Write, `persist()` 호출 시 즉시 저장
- [ ] Pretty-print 직렬화 (들여쓰기 배열 형식)

#### 1-C. ConsoleUtil
- [ ] ANSI VT100 색상 헬퍼 (초록/노랑/빨강/리셋)
- [ ] 정수 입력 파싱 유틸 (`readInt`, `readLine`)

### TDD 대상
- `JsonParser`: 문자열 → JsonValue 파싱 (string, int, double, array, object, nested)
- `JsonStorage`: `save` → 파일 생성, `load` → 동일 데이터 복원 (라운드트립)

### 검증
- 모든 JsonParser / JsonStorage 단위 테스트 GREEN

---

## Phase 2 — 도메인 모델

### 목표
비즈니스 엔티티 정의 (순수 데이터 + Enum, 의존성 없음)

### 작업 항목

#### 2-A. Sample
```cpp
struct Sample {
    std::string sampleId;       // S-001
    std::string name;
    double      avgProductionTime; // min/ea
    double      yieldRate;         // 0.0 ~ 1.0
    int         stock;
};
```

#### 2-B. Order + OrderStatus
```cpp
enum class OrderStatus { RESERVED, REJECTED, PRODUCING, CONFIRMED, RELEASED };

struct Order {
    std::string  orderId;        // ORD-YYYYMMDD-NNNN
    std::string  sampleId;
    std::string  customerName;
    int          quantity;
    OrderStatus  status;
    std::string  createdAt;
    std::string  updatedAt;
};
```

#### 2-C. ProductionJob + JobStatus
```cpp
enum class JobStatus { WAITING, RUNNING, DONE };

struct ProductionJob {
    std::string jobId;
    std::string orderId;
    std::string sampleId;
    int         shortage;
    int         actualProd;
    double      totalTime;
    JobStatus   status;
    std::string enqueuedAt;
    std::string startedAt;
    std::string completedAt;
};
```

#### 2-D. JSON 직렬화/역직렬화
- 각 모델의 `toJson()` / `fromJson()` 구현

### TDD 대상
- `Order::toJson()` / `Order::fromJson()` 라운드트립
- `OrderStatus` 문자열 변환 (RESERVED ↔ "RESERVED")
- `ProductionJob::toJson()` / `ProductionJob::fromJson()` 라운드트립

### 검증
- 모든 모델 직렬화 단위 테스트 GREEN

---

## Phase 3 — Repository 계층

### 목표
**Repository 인터페이스** 정의 + JSON 파일 기반 CRUD 구현 (DataPersistence-Andrew 패턴)

### 작업 항목

#### 3-A. Repository 인터페이스 정의 (신규 — Mock 주입의 기반)
- [ ] `include/ISampleRepository.h` — `findAll()`, `findById()`, `save()`, `update()`, `generateNextId()`
- [ ] `include/IOrderRepository.h` — `findAll()`, `findById()`, `findByStatus()`, `save()`, `update()`, `getNextSequence()`
- [ ] `include/IProductionJobRepository.h` — `findAll()`, `findById()`, `findByStatus()`, `findRunning()`, `findWaiting()`, `save()`, `update()`, `generateNextId()`

#### 3-B. 구체 Repository 구현 (인터페이스 상속)
- [ ] `SampleRepository : public ISampleRepository`
- [ ] `OrderRepository : public IOrderRepository`
- [ ] `ProductionJobRepository : public IProductionJobRepository`

### TDD 대상
- Repository 테스트: `TEST_F` + `SetUp`/`TearDown` (임시 파일 격리)
- `OrderRepository::getNextSequence`: 같은 날 순번 단조 증가, 재시작 후 유지
- `OrderRepository::findByStatus(RESERVED)`: RESERVED만 반환
- `SampleRepository::update`: stock 변경 후 `findById`로 일치 확인

### 검증
- Repository 단위 테스트 GREEN + `data/*.json` 파일 생성 확인
- `ISampleRepository`, `IOrderRepository`, `IProductionJobRepository` 인터페이스 컴파일 확인

---

## Phase 4 — 메인 메뉴

### 목표
시스템 진입 화면 및 메뉴 루프 구현 (더미 서브 메뉴 포함)

### 작업 항목
- [ ] `MainView::printHeader()` — 현재 날짜·시간, 시스템 현황 요약 출력
  - 등록 시료 수 / 총 재고 / 전체 주문 수(REJECTED 제외) / 생산라인 대기 수
- [ ] `MainView::printMenu()` — 메뉴 번호 출력
- [ ] `MainController(ISampleRepository&, IOrderRepository&, IProductionJobRepository&)` — 인터페이스 의존
- [ ] `MainController::run()` — 입력 루프 (0~6, 범위 외 재입력)
- [ ] `SampleOrderSystemTests/mocks/MockRepositories.h` — 3종 Mock 정의 (Phase 5~10 공용)

### TDD 대상
- `MainController`: 잘못된 입력(7, -1, "abc") → 오류 메시지 출력, 루프 유지 (Mock 주입)
- `MainController`: 0 입력 → 종료 플래그 (Mock 주입)
- `buildStatus()`: Mock이 반환하는 데이터로 집계 정확성 검증

### 검증
- 메인 메뉴 빌드 & 실행, 0~6 입력 동작, 잘못된 입력 처리
- `MockRepositories.h` 컴파일 + Mock 메서드 호출 검증

---

## Phase 5 — 시료 관리

### 목표
시료 등록 / 목록 조회 / 이름 검색 기능 구현

### 작업 항목
- [ ] `SampleController(ISampleRepository&)` — 인터페이스 의존
- [ ] `SampleController::registerSample()` — 이름·생산시간·수율·초기재고 입력 → 저장
  - 유효성: 이름 공백 불가, 수율 0 < x ≤ 1.0, 생산시간 > 0, 재고 ≥ 0
  - ID 자동 생성: `S-{3자리 순번}`
- [ ] `SampleController::listSamples()` — 전체 목록 출력
- [ ] `SampleController::searchSamples()` — 이름 부분 일치 검색
- [ ] `SampleView` — 등록 폼, 목록 테이블, 검색 결과 출력

### TDD 대상 (MockSampleRepository 사용)
- `registerSample`: 수율 범위 초과 → `save()` 0회 호출 (EXPECT_CALL)
- `registerSample`: 정상 입력 → `save()` 1회 호출, `generateNextId()` 호출
- `searchSamples`: Mock `findAll()` 반환 데이터로 부분 문자열 필터 검증

### 검증
- 단위 테스트 GREEN (MockSampleRepository)
- 통합 테스트: 시료 등록 후 목록 조회 확인, 재시작 후 데이터 유지

---

## Phase 6 — 시료 주문 접수

### 목표
고객 주문을 RESERVED 상태로 접수하고 주문번호 자동 생성

### 작업 항목
- [ ] `OrderController(ISampleRepository&, IOrderRepository&)` — Phase 6 초기 시그니처 (Phase 7에서 `IProductionJobRepository&` 추가로 확장)
- [ ] `OrderController::placeOrder()` — 시료 선택 → 고객명 → 수량 입력 → RESERVED 저장
  - 시스템에 등록된 시료만 선택 가능
  - 수량 > 0 유효성 검사
- [ ] `OrderIdGenerator::generate(date, seq)` — 순수 함수
- [ ] `OrderView` — 시료 선택 목록, 주문 입력 폼, 접수 완료 출력

### TDD 대상
- `OrderIdGenerator`: 순수 함수 → `TEST()` 직접 사용
- `placeOrder`: 빈 시료 목록 → `save()` 0회 (MockSampleRepository + MockOrderRepository)
- `placeOrder`: 정상 접수 → `save()` 1회, RESERVED 상태 주문 전달 검증

### 검증
- 단위 테스트 GREEN
- 통합 테스트: 접수 후 `data/orders.json`에 RESERVED 상태로 저장 확인

---

## Phase 7 — 주문 승인/거절

### 목표
재고 분기 로직 + ProductionQueue FIFO 구현 (핵심 비즈니스 로직)

### 작업 항목

#### 7-A. 재고 분기 로직
- [ ] `OrderController(ISampleRepository&, IOrderRepository&, IProductionJobRepository&)` — 3종 인터페이스 의존
- [ ] `OrderController::approveOrder()`:
  - 재고 충분 (stock >= quantity): `stock -= quantity`, 상태 → CONFIRMED
  - 재고 부족 (stock < quantity): ProductionJob 생성, 큐 등록, 상태 → PRODUCING
- [ ] `OrderController::rejectOrder()`: 상태 → REJECTED

#### 7-B. 생산량 계산
- [ ] `ProductionCalculator::calculate(...)` — 순수 함수, 외부 의존 없음

#### 7-C. ProductionQueue
- [ ] `ProductionQueue(IProductionJobRepository&)` — 인터페이스 의존 → Mock 주입 가능
- [ ] `enqueue(job)`: WAITING → 큐 비어있으면 즉시 RUNNING
- [ ] `completeCurrentJob()`: RUNNING → DONE, 다음 WAITING → RUNNING

#### 7-D. View
- [ ] `OrderView` — RESERVED 목록, 상세 확인, 승인/거절 결과 출력

### TDD 대상 (핵심)
- `ProductionCalculator`: 순수 함수 → `TEST()` 직접 사용
  - 부족분 170, 수율 0.92 → `actualProd == 206`, `totalTime == 164.8`
  - 부족분 1, 수율 1.0 → `actualProd == 2` (ceil 확인)
- `ProductionQueue`: `MockProductionJobRepository` 주입
  - 첫 번째 enqueue → `findRunning()` nullopt → `update()` 호출 (RUNNING 전환)
  - 두 번째 enqueue → `findRunning()` 반환 있음 → `update()` 미호출
  - `completeCurrentJob` → `update()` 2회 (DONE + RUNNING 전환)
- 승인/거절: `MockSampleRepository`, `MockOrderRepository`, `MockProductionJobRepository` 3종 주입
  - 재고 충분: `sampleRepo.update()` 1회, `orderRepo.update()` CONFIRMED
  - 재고 부족: `jobRepo.save()` 1회, `sampleRepo.update()` 0회, PRODUCING
  - 거절: `sampleRepo.update()` 0회, `orderRepo.update()` REJECTED

### 검증
- 단위 테스트 GREEN (Mock 기반)
- 통합 테스트: 승인 시나리오 2종(충분/부족) + 거절 E2E 동작 확인

---

## Phase 8 — 모니터링

### 목표
상태별 주문 집계 및 시료별 재고 상태(여유/부족/고갈) 표시

### 작업 항목
- [ ] `MonitoringController(ISampleRepository&, IOrderRepository&)` — 인터페이스 의존
- [ ] `evaluateInventory()`, `calcRemainingRatio()` — 순수 함수
- [ ] `MonitoringController::showOrderStats()` — REJECTED 제외 상태별 집계
- [ ] `MonitoringController::showInventoryStatus()` — 시료별 재고 상태 + ANSI 색상

### TDD 대상
- `evaluateInventory`, `calcRemainingRatio`: 순수 함수 → `TEST()` 직접 사용
- `buildOrderStats()`: `MockOrderRepository.findAll()` 주입 → REJECTED 제외 검증
- `buildInventoryRows()`: `MockSampleRepository.findAll()` + `MockOrderRepository.findAll()` 주입 → PRODUCING 제외 검증

### 검증
- 단위 테스트 GREEN (Mock 2종)
- 색상 구분 화면 출력 확인 (여유/부족/고갈 각 1건 이상)

---

## Phase 9 — 생산라인

### 목표
FIFO 생산 큐 현황 표시 및 수동 생산 완료 처리

### 작업 항목
- [ ] `ProductionController(ISampleRepository&, IOrderRepository&, IProductionJobRepository&)` — 3종 인터페이스 의존
- [ ] `ProductionController::completeCurrentJob()` — 수동 완료 처리
  - `addedStock = floor(actualProd × yieldRate)`
  - `sample.stock += addedStock`
  - 주문 상태: PRODUCING → CONFIRMED
  - ProductionJob 상태: RUNNING → DONE + 다음 WAITING → RUNNING

### TDD 대상 (Mock 3종 사용)
- `completeCurrentJob`: `sampleRepo.update()` 검증 — stock = 기존 + floor(actualProd × yieldRate)
- `completeCurrentJob`: `orderRepo.update()` 검증 — status = CONFIRMED
- `completeCurrentJob`: `jobRepo.update()` 검증 — DONE + 다음 RUNNING 전환
- RUNNING 없음: `sampleRepo.update()` 0회, 안내 메시지

### 검증
- 단위 테스트 GREEN (Mock 3종)
- 통합 테스트: 2개 PRODUCING 주문 순차 완료 시 FIFO 순서 확인

---

## Phase 10 — 출고 처리

### 목표
CONFIRMED 주문에 대한 출고 실행 (CONFIRMED → RELEASED)

### 작업 항목
- [ ] `ReleaseController(ISampleRepository&, IOrderRepository&)` — 인터페이스 의존
- [ ] `ReleaseController::processRelease()` — 상태 RELEASED 전환, updatedAt 기록
- [ ] `ReleaseView` — CONFIRMED 목록, 처리 결과 출력
- [ ] 출고 시 재고 추가 차감 없음 (재고는 승인 시점에 이미 차감)

### TDD 대상 (Mock 2종 사용)
- `processRelease`: `orderRepo.update()` RELEASED 상태 검증
- `processRelease`: `sampleRepo.update()` 0회 (stock 차감 없음) 검증
- CONFIRMED 없음: `orderRepo.update()` 0회, 안내 메시지
- [N] 취소: `orderRepo.update()` 0회

### 검증
- 단위 테스트 GREEN (Mock 2종)
- 통합 테스트: CONFIRMED → RELEASED 전이 후 모니터링에서 RELEASED 카운트 증가 확인

---

## Phase 11 — 통합 & 마무리

### 목표
전체 플로우 통합 검증, UI 완성도, 더미 데이터 도구

### 작업 항목

#### 11-A. 통합 테스트 (실제 Repository 사용 — Mock 없음)
- [ ] `TEST_F` + SetUp/TearDown으로 임시 파일 격리
- [ ] 주문 접수 → 승인(재고 충분) → 출고 전체 플로우
- [ ] 주문 접수 → 승인(재고 부족) → 생산 완료 → 출고 전체 플로우
- [ ] 여러 PRODUCING 주문 FIFO 순서 검증

#### 11-B. UI 완성도
- [ ] 메인 메뉴 현황 요약 실제 데이터 연결
- [ ] 모든 화면 컬럼 정렬 일관성 확인
- [ ] ANSI 색상 Windows 콘솔 활성화 (`SetConsoleMode`)

#### 11-C. 더미 데이터 생성 (DummyDataGenerator-Andrew 참조)
- [ ] Sample 더미 4종
- [ ] Order 더미 10건 이상 (다양한 상태)

#### 11-D. 최종 정리
- [ ] 미사용 코드 제거
- [ ] `RUN_ALL_TESTS()` 전체 테스트 GREEN 확인
- [ ] README 작성

### 검증
- 전체 시나리오 E2E 실행 + 모든 단위/통합 테스트 GREEN (`RUN_ALL_TESTS()`)

---

## 의존성 다이어그램

```
Phase 0 (셋업)
    └── Phase 1 (JsonStorage / 인터페이스)
    └── Phase 2 (도메인 모델)
            └── Phase 3 (Repository)
                    └── Phase 4 (메인 메뉴)
                            ├── Phase 5 (시료 관리)
                            ├── Phase 6 (주문 접수)
                            │       └── Phase 7 (승인/거절) ← 핵심
                            ├── Phase 8 (모니터링)
                            ├── Phase 9 (생산라인) ← Phase 7 이후
                            └── Phase 10 (출고 처리) ← Phase 7 이후
                                        └── Phase 11 (통합 마무리)
```

---

## 병렬 진행 가능 구간

| 구간 | 병렬 가능 Phase |
|------|----------------|
| Phase 3 완료 후 | Phase 5 / Phase 6 / Phase 8 동시 진행 가능 |
| Phase 7 완료 후 | Phase 9 / Phase 10 동시 진행 가능 |

---

## gmock TDD 전략

### Repository 인터페이스 + Mock 주입 패턴

```
Controller (ISampleRepository& 의존)
    ↑ 프로덕션: SampleRepository (실제 파일 I/O)
    ↑ 테스트:   MockSampleRepository (gmock)
```

**인터페이스 정의** (Phase 3에서):
- `ISampleRepository`, `IOrderRepository`, `IProductionJobRepository`

**Mock 정의** (Phase 4에서 공용 헤더로):
- `SampleOrderSystemTests/mocks/MockRepositories.h`
- gmock `MOCK_METHOD` 매크로 사용

**테스트 계층별 전략**:
| 계층 | 테스트 방법 | 파일 |
|------|------------|------|
| 순수 함수 (ProductionCalculator, OrderIdGenerator, InventoryStatus) | `TEST()` 직접 | `test_production_calculator.cpp` 등 |
| Repository (실제 파일 I/O) | `TEST_F` + SetUp/TearDown (임시 파일) | `test_*_repository.cpp` |
| Controller (Repository 의존) | `TEST_F` + MockRepository 주입 | `test_*_controller.cpp` |
| 통합 | `TEST_F` + 실제 Repository (임시 파일) | `test_integration.cpp` |

**테스트 파일 경로**: 모든 테스트는 `SampleOrderSystemTests/` 내에 위치.  
`tests/` 디렉터리는 사용하지 않는다.

---

## 주의사항

- **Phase 3** — Repository 인터페이스 정의가 Phase 4 이후 모든 Controller 의존성 주입의 기반. 인터페이스를 먼저 확정한다.
- **Phase 7 (승인/거절)** 은 생산량 계산·상태 전이·재고 분기가 모두 집중된 핵심 Phase. 테스트 케이스를 가장 충실히 작성한다.
- `data/` 디렉터리는 `.gitignore`에서 제외 (런타임 생성, 커밋 불필요).
- ANSI 색상은 Windows 10+ 기준 `ENABLE_VIRTUAL_TERMINAL_PROCESSING` 플래그 활성화 필요.
- 주문번호 순번은 `data/orders.json`의 마지막 순번에서 이어받는 방식으로 재시작 후에도 유지.
- **자체 테스트 매크로(TestRunner.h) 사용 금지** — gmock/gtest 전용.
