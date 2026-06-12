# Phase 11 — 통합 & 마무리

## 목표
전체 플로우 통합 검증, UI 완성도 점검, 더미 데이터 도구 구현, 최종 정리.

---

## 선행 조건
- Phase 5~10 모두 완료 (모든 기능 구현)

---

## 작업 항목

### 11-A. 통합 테스트

**파일**: `SampleOrderSystemTests/test_integration.cpp`

통합 테스트는 실제 구체 Repository 사용 (Mock 없이 실제 파일 I/O).  
테스트 전용 경로를 `SetUp`/`TearDown`으로 격리.

```cpp
// SampleOrderSystemTests/test_integration.cpp
#include <gmock/gmock.h>
#include <filesystem>
#include "repository/SampleRepository.h"
#include "repository/OrderRepository.h"
#include "repository/ProductionJobRepository.h"
#include "model/ProductionCalculator.h"
#include "model/ProductionQueue.h"

class IntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        samplePath_ = "data/integration_samples_" + std::to_string(counter_) + ".json";
        orderPath_  = "data/integration_orders_" + std::to_string(counter_) + ".json";
        jobPath_    = "data/integration_jobs_" + std::to_string(counter_) + ".json";
        counter_++;
        sampleRepo_ = std::make_unique<SampleRepository>(samplePath_);
        orderRepo_  = std::make_unique<OrderRepository>(orderPath_);
        jobRepo_    = std::make_unique<ProductionJobRepository>(jobPath_);
    }
    void TearDown() override {
        std::filesystem::remove(samplePath_);
        std::filesystem::remove(orderPath_);
        std::filesystem::remove(jobPath_);
    }
    std::string samplePath_, orderPath_, jobPath_;
    std::unique_ptr<SampleRepository>        sampleRepo_;
    std::unique_ptr<OrderRepository>         orderRepo_;
    std::unique_ptr<ProductionJobRepository> jobRepo_;
    static int counter_;
};
int IntegrationTest::counter_ = 0;
```

#### 시나리오 1: 재고 충분 전체 플로우
```
1. Sample 등록 (stock=500)
2. 주문 접수 (qty=100) → RESERVED
3. 주문 승인 → CONFIRMED (stock=400)
4. 출고 처리 → RELEASED (stock=400 유지)
```

```cpp
TEST_F(IntegrationTest, StockSufficientFullFlow) {
    // 1. Sample 등록
    Sample s{"S-001", "웨이퍼", 0.8, 0.92, 500};
    sampleRepo_->save(s);

    // 2. 주문 접수
    Order o; o.orderId = "ORD-20260612-0001";
    o.sampleId = "S-001"; o.quantity = 100; o.status = OrderStatus::RESERVED;
    orderRepo_->save(o);

    // 3. 승인 (재고 충분)
    s.stock -= 100;
    sampleRepo_->update(s);
    o.status = OrderStatus::CONFIRMED;
    orderRepo_->update(o);

    EXPECT_EQ(sampleRepo_->findById("S-001")->stock, 400);
    EXPECT_EQ(orderRepo_->findById("ORD-20260612-0001")->status, OrderStatus::CONFIRMED);

    // 4. 출고
    o.status = OrderStatus::RELEASED;
    orderRepo_->update(o);

    EXPECT_EQ(sampleRepo_->findById("S-001")->stock, 400);  // stock 변동 없음
    EXPECT_EQ(orderRepo_->findById("ORD-20260612-0001")->status, OrderStatus::RELEASED);
}
```

검증 포인트:
- 각 단계별 status 정확성
- stock 변동: 500 → 400 → 400

#### 시나리오 2: 재고 부족 전체 플로우
```
1. Sample 등록 (stock=30)
2. 주문 접수 (qty=200) → RESERVED
3. 주문 승인 → PRODUCING, ProductionJob 생성 (RUNNING)
4. 생산 완료 처리 → CONFIRMED, stock += floor(actualProd × yieldRate)
5. 출고 처리 → RELEASED
```

```cpp
TEST_F(IntegrationTest, StockInsufficientFullFlow) {
    Sample s{"S-001", "SiC", 0.8, 0.92, 30};
    sampleRepo_->save(s);

    Order o; o.orderId = "ORD-20260612-0001";
    o.sampleId = "S-001"; o.quantity = 200; o.status = OrderStatus::RESERVED;
    orderRepo_->save(o);

    // 생산량 계산
    auto calc = ProductionCalculator::calculate(200, 30, 0.92, 0.8);
    EXPECT_EQ(calc.shortage, 170);
    EXPECT_EQ(calc.actualProd, 206);

    // 생산 작업 생성
    ProductionJob job;
    job.jobId = "JOB-20260612-0001"; job.orderId = "ORD-20260612-0001";
    job.sampleId = "S-001"; job.shortage = calc.shortage; job.actualProd = calc.actualProd;
    job.totalTime = calc.totalTime; job.status = JobStatus::RUNNING;
    jobRepo_->save(job);

    o.status = OrderStatus::PRODUCING;
    orderRepo_->update(o);

    // 생산 완료
    int addedStock = static_cast<int>(std::floor(206 * 0.92));  // 189
    s.stock += addedStock;
    sampleRepo_->update(s);
    o.status = OrderStatus::CONFIRMED;
    orderRepo_->update(o);
    job.status = JobStatus::DONE;
    jobRepo_->update(job);

    EXPECT_EQ(sampleRepo_->findById("S-001")->stock, 30 + 189);  // 219
    EXPECT_EQ(orderRepo_->findById("ORD-20260612-0001")->status, OrderStatus::CONFIRMED);

    // 출고
    o.status = OrderStatus::RELEASED;
    orderRepo_->update(o);
    EXPECT_EQ(orderRepo_->findById("ORD-20260612-0001")->status, OrderStatus::RELEASED);
}
```

#### 시나리오 3: FIFO 다중 생산 주문
```
1. PRODUCING 주문 3건 생성 (job1, job2, job3 순서)
2. job1 완료 → job2 자동 RUNNING
3. job2 완료 → job3 자동 RUNNING
4. job3 완료 → 큐 비어있음
```

```cpp
TEST_F(IntegrationTest, FifoOrder) {
    ProductionQueue queue(*jobRepo_);

    ProductionJob job1, job2, job3;
    job1.jobId = "JOB-20260612-0001"; job1.status = JobStatus::WAITING;
    job2.jobId = "JOB-20260612-0002"; job2.status = JobStatus::WAITING;
    job3.jobId = "JOB-20260612-0003"; job3.status = JobStatus::WAITING;

    queue.enqueue(job1);  // → RUNNING
    queue.enqueue(job2);  // → WAITING
    queue.enqueue(job3);  // → WAITING

    EXPECT_EQ(jobRepo_->findRunning()->jobId, "JOB-20260612-0001");
    queue.completeCurrentJob();
    EXPECT_EQ(jobRepo_->findRunning()->jobId, "JOB-20260612-0002");
    queue.completeCurrentJob();
    EXPECT_EQ(jobRepo_->findRunning()->jobId, "JOB-20260612-0003");
    queue.completeCurrentJob();
    EXPECT_FALSE(jobRepo_->findRunning().has_value());  // 큐 비어있음
}
```

검증 포인트:
- 완료 순서 == 등록 순서 (FIFO)

#### 시나리오 4: 거절 흐름
```
1. 주문 접수 → RESERVED
2. 주문 거절 → REJECTED
3. 모니터링 집계 → REJECTED 제외 확인
```

---

### 11-B. 메인 메뉴 현황 요약 연결

Phase 4에서 구현한 `MainController::buildStatus()`에 실제 Repository 연결 검증:

| 항목 | 데이터 출처 |
|------|------------|
| 등록 시료 수 | `sampleRepo.findAll().size()` |
| 총 재고 | `sampleRepo.findAll()` stock 합산 |
| 전체 주문 수 | `orderRepo.findAll()` — REJECTED 제외 |
| 생산라인 대기 수 | `jobRepo.findByStatus(JobStatus::WAITING).size()` |

---

### 11-C. 더미 데이터 생성 도구 (DummyDataGenerator-Andrew 참조)

**파일**: `src/tools/DummyDataGenerator.h`, `src/tools/DummyDataGenerator.cpp`

```cpp
class DummyDataGenerator {
public:
    DummyDataGenerator(
        SampleRepository&        sampleRepo,
        OrderRepository&         orderRepo,
        ProductionJobRepository& jobRepo
    );

    void generateAll();          // 전체 더미 데이터 생성
    void generateSamples();      // 시료 4종
    void generateOrders();       // 주문 10건 이상 (다양한 상태)
    void clearAll();             // 전체 데이터 초기화 (테스트용)

private:
    SampleRepository&        sampleRepo_;
    OrderRepository&         orderRepo_;
    ProductionJobRepository& jobRepo_;
};
```

생성할 더미 Sample:
| sampleId | name | avgTime | yieldRate | stock |
|----------|------|---------|-----------|-------|
| S-001 | 실리콘 웨이퍼-8인치 | 0.8 | 0.92 | 480 |
| S-002 | GaN 에피택셀-4인치 | 0.3 | 0.88 | 220 |
| S-003 | SiC 파워기판-6인치 | 0.8 | 0.92 | 30 |
| S-004 | 산화막 웨이퍼-SiO2 | 0.6 | 0.95 | 0 |

생성할 더미 Order (10건):
| 고객 | 시료 | 수량 | 상태 |
|------|------|------|------|
| SK하이닉스 | S-001 | 150 | RESERVED |
| LG이노텍 | S-004 | 300 | RESERVED |
| 삼성전자 | S-003 | 200 | PRODUCING |
| DB하이텍 | S-002 | 100 | CONFIRMED |
| ... | ... | ... | RELEASED × 3 |

---

### 11-D. UI 완성도 점검

- [ ] 모든 화면 컬럼 정렬 일관성 (테이블 헤더 ↔ 데이터 정렬)
- [ ] Windows 10+ 콘솔 ANSI 색상 정상 동작 (`ENABLE_VIRTUAL_TERMINAL_PROCESSING`)
- [ ] ANSI 미지원 환경에서도 기능 정상 동작 (색상 없이 텍스트만)
- [ ] 긴 시료명 / 고객명 처리 (컬럼 너비 넘침 방지)
- [ ] 모든 `선택 > ` 프롬프트 일관성 확인

---

### 11-E. 최종 정리

- [ ] 미사용 include, 변수, 함수 제거
- [ ] 모든 단위 테스트 GREEN 재확인 (`RUN_ALL_TESTS()`)
- [ ] 통합 테스트 시나리오 1~4 모두 GREEN
- [ ] `data/` 디렉터리 `.gitignore` 확인
- [ ] `temp_docs/` 폴더 처리 방침 결정 (커밋 또는 제외)

---

## TDD 테스트 케이스

### 통합 시나리오

| # | 시나리오 | 기대 결과 |
|---|----------|----------|
| T1 | 시나리오 1 (재고 충분) 전체 | 각 단계 status 정확, stock 400 |
| T2 | 시나리오 2 (재고 부족) 전체 | stock 증가 219, 최종 RELEASED |
| T3 | 시나리오 3 (FIFO 3건) | 완료 순서 == 등록 순서 |
| T4 | 시나리오 4 (거절) | 모니터링 집계에서 REJECTED 제외 |

---

## 산출물

| 파일 | 역할 |
|------|------|
| `SampleOrderSystemTests/test_integration.cpp` | 전체 플로우 통합 테스트 (실제 파일 I/O, TEST_F) |
| `src/tools/DummyDataGenerator.h/.cpp` | 더미 데이터 생성 도구 |

---

## 검증 기준
- [ ] 통합 테스트 T1~T4 모두 GREEN
- [ ] 전체 단위 테스트 GREEN (0 fail) — `RUN_ALL_TESTS()` 실행
- [ ] 더미 데이터 생성 후 모든 기능 화면 정상 출력
- [ ] ANSI 색상 Windows 콘솔 동작 확인
- [ ] 빌드 경고 0건 확인

---

## 비고
- 통합 테스트는 실제 `SampleRepository`, `OrderRepository`, `ProductionJobRepository` 사용 (Mock 없음)
- 테스트 파일 경로를 `SetUp`/`TearDown`으로 격리 → 실 데이터 오염 방지
- 더미 데이터 생성은 별도 메뉴 또는 초기화 함수로 노출 (선택)
- `temp_docs/`는 개발 계획 문서 — 최종 커밋 여부는 팀 결정에 따름
