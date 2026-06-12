# Phase 4 — 메인 메뉴

## 목표
시스템 진입 화면 및 메뉴 루프 구현.  
각 서브 메뉴는 이후 Phase에서 교체될 placeholder로 연결.

---

## 선행 조건
- Phase 1 완료 (`ConsoleUtil`, `IController`, `IView`)
- Phase 2 완료 (도메인 모델)
- Phase 3 완료 (Repository 인터페이스 + 구체 구현)

---

## 작업 항목

### 4-A. MainView

**파일**: `src/view/MainView.h`, `src/view/MainView.cpp`

```cpp
class MainView : public IView {
public:
    struct SystemStatus {
        std::string currentDateTime;
        int sampleCount;
        int totalStock;
        int totalOrderCount;      // REJECTED 제외
        int productionQueueCount; // WAITING 상태 생산 작업 건수 (JobRepo 기준)
    };

    void render() override;  // printHeader + printMenu 순서로 호출
    void printHeader(const SystemStatus& status);
    void printMenu();
    void printInvalidInput();

    int getMenuInput();  // 0~6 입력 대기
};
```

출력 예시:
```
========================================
  S-Semi 반도체 시료 생산주문관리 시스템
========================================
  날짜/시간 : 2026-06-12 09:00:00
  등록 시료 : 4종  |  총 재고 : 730 ea
  전체 주문 : 32건  |  생산 대기 : 3건
========================================

  [1] 시료 관리
  [2] 시료 주문
  [3] 주문 승인/거절
  [4] 모니터링
  [5] 생산라인 조회
  [6] 출고 처리
  [0] 종료

선택 > 
```

---

### 4-B. MainController

**파일**: `src/controller/MainController.h`, `src/controller/MainController.cpp`

```cpp
// Controller는 인터페이스에만 의존 → Mock 주입 가능
class MainController : public IController {
public:
    MainController(
        ISampleRepository&        sampleRepo,
        IOrderRepository&         orderRepo,
        IProductionJobRepository& jobRepo
    );

    void run() override;

private:
    SystemStatus buildStatus() const;
    void handleMenu(int choice);

    ISampleRepository&        sampleRepo_;
    IOrderRepository&         orderRepo_;
    IProductionJobRepository& jobRepo_;
    MainView                  view_;
    bool                      running_ = true;
};
```

루프 동작:
```
while (running_) {
    SystemStatus status = buildStatus();
    view_.printHeader(status);
    view_.printMenu();
    int choice = view_.getMenuInput();
    handleMenu(choice);
}
```

`handleMenu(choice)` 매핑:
| 입력 | 동작 |
|------|------|
| 0 | `running_ = false` |
| 1~6 | 해당 Controller 호출 (Phase 5~10에서 구현, 현재는 placeholder) |
| 그 외 | `view_.printInvalidInput()` |

---

### 4-C. main.cpp 업데이트

```cpp
int main() {
    ConsoleUtil::enableVirtualTerminal();

    SampleRepository        sampleRepo;
    OrderRepository         orderRepo;
    ProductionJobRepository jobRepo;

    // 구체 클래스가 인터페이스를 구현하므로 업캐스팅으로 전달
    MainController controller(sampleRepo, orderRepo, jobRepo);
    controller.run();

    return 0;
}
```

---

## TDD 테스트 케이스

### Mock 정의 (공용 헤더 또는 테스트 파일 내 인라인 정의)

```cpp
// SampleOrderSystemTests/mocks/MockRepositories.h
#include <gmock/gmock.h>
#include "include/ISampleRepository.h"
#include "include/IOrderRepository.h"
#include "include/IProductionJobRepository.h"

class MockSampleRepository : public ISampleRepository {
public:
    MOCK_METHOD(std::vector<Sample>,   findAll,        (), (const, override));
    MOCK_METHOD(std::optional<Sample>, findById,       (const std::string&), (const, override));
    MOCK_METHOD(void,                  save,           (const Sample&), (override));
    MOCK_METHOD(void,                  update,         (const Sample&), (override));
    MOCK_METHOD(std::string,           generateNextId, (), (const, override));
};

class MockOrderRepository : public IOrderRepository {
public:
    MOCK_METHOD(std::vector<Order>,   findAll,          (), (const, override));
    MOCK_METHOD(std::optional<Order>, findById,         (const std::string&), (const, override));
    MOCK_METHOD(std::vector<Order>,   findByStatus,     (OrderStatus), (const, override));
    MOCK_METHOD(void,                 save,             (const Order&), (override));
    MOCK_METHOD(void,                 update,           (const Order&), (override));
    MOCK_METHOD(int,                  getNextSequence,  (const std::string&), (const, override));
};

class MockProductionJobRepository : public IProductionJobRepository {
public:
    MOCK_METHOD(std::vector<ProductionJob>,   findAll,         (), (const, override));
    MOCK_METHOD(std::optional<ProductionJob>, findById,        (const std::string&), (const, override));
    MOCK_METHOD(std::vector<ProductionJob>,   findByStatus,    (JobStatus), (const, override));
    MOCK_METHOD(std::optional<ProductionJob>, findRunning,     (), (const, override));
    MOCK_METHOD(std::vector<ProductionJob>,   findWaiting,     (), (const, override));
    MOCK_METHOD(void,                         save,            (const ProductionJob&), (override));
    MOCK_METHOD(void,                         update,          (const ProductionJob&), (override));
    MOCK_METHOD(std::string,                  generateNextId,  (), (const, override));
};
```

### MainController 입력 처리

```cpp
// SampleOrderSystemTests/test_main_controller.cpp
#include <gmock/gmock.h>
#include "mocks/MockRepositories.h"
#include "controller/MainController.h"

using ::testing::Return;
using ::testing::ReturnRef;

class MainControllerTest : public ::testing::Test {
protected:
    MockSampleRepository        mockSampleRepo_;
    MockOrderRepository         mockOrderRepo_;
    MockProductionJobRepository mockJobRepo_;
};
```

| # | 입력 | 기대 결과 |
|---|------|----------|
| T1 | `0` 입력 | `running_` = false, 루프 종료 |
| T2 | `7` 입력 | `printInvalidInput()` 호출, 루프 유지 |
| T3 | `-1` 입력 | `printInvalidInput()` 호출, 루프 유지 |
| T4 | 문자 `"abc"` 입력 | `printInvalidInput()` 호출, 루프 유지 |
| T5 | `1`~`6` 입력 | placeholder 진입 후 메인 메뉴 복귀 |

### SystemStatus 빌드 (Mock Repository 사용)

```cpp
TEST_F(MainControllerTest, BuildStatusCountsSamplesCorrectly) {
    Sample s1{"S-001", "웨이퍼", 0.8, 0.92, 150};
    Sample s2{"S-002", "GaN", 0.3, 0.88, 50};
    EXPECT_CALL(mockSampleRepo_, findAll())
        .WillOnce(Return(std::vector<Sample>{s1, s2}));
    EXPECT_CALL(mockOrderRepo_, findAll())
        .WillOnce(Return(std::vector<Order>{}));
    EXPECT_CALL(mockJobRepo_, findByStatus(JobStatus::WAITING))
        .WillOnce(Return(std::vector<ProductionJob>{}));

    MainController ctrl(mockSampleRepo_, mockOrderRepo_, mockJobRepo_);
    auto status = ctrl.buildStatus();  // private이면 friend 또는 별도 접근 방법 사용
    EXPECT_EQ(status.sampleCount, 2);
    EXPECT_EQ(status.totalStock, 200);
}
```

| # | 시나리오 | 기대 결과 |
|---|----------|----------|
| T6 | Mock: Sample 3건, 총 재고 200 | `sampleCount=3`, `totalStock=200` |
| T7 | Mock: REJECTED 주문 포함 5건, REJECTED 2건 | `totalOrderCount=3` |
| T8 | Mock: WAITING 작업 2건 | `productionQueueCount=2` |

---

## 산출물

| 파일 | 역할 |
|------|------|
| `src/view/MainView.h/.cpp` | 헤더 + 메뉴 출력 |
| `src/controller/MainController.h/.cpp` | 메뉴 루프 제어 (ISampleRepository& 등 인터페이스 의존) |
| `main.cpp` | 구체 Repository 생성 + MainController 실행 |
| `SampleOrderSystemTests/mocks/MockRepositories.h` | 3종 Mock Repository 정의 (Phase 4~10 공용) |
| `SampleOrderSystemTests/test_main_controller.cpp` | 입력 처리 + 현황 집계 단위 테스트 |

---

## 검증 기준
- [ ] T1~T8 모든 테스트 GREEN (Mock Repository 사용)
- [ ] 메뉴 루프 실행 → 0 입력 시 정상 종료
- [ ] 잘못된 입력 연속 3회 → 오류 메시지 3회 출력, 루프 유지
- [ ] 현황 요약에 실제 Repository 데이터 반영 확인 (통합 테스트)
- [ ] REFACTOR 검토: `buildStatus()` 복잡도 및 책임 분리 여부

---

## 비고
- `MockRepositories.h`는 Phase 5~10에서도 공용으로 재사용
- 서브 메뉴 placeholder는 단순 `std::cout << "미구현\n"` 수준으로 충분
- `getMenuInput()`은 비숫자 입력 시 -1 반환 (예외 발생 금지)
- `buildStatus()`가 private인 경우 `friend class MainControllerTest` 선언 또는 반환값을 통해 간접 검증
