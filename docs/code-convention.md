# Code Convention (C++)

C++17 표준 및 PoC 코드베이스 스타일을 기반으로 한다.
Google C++ Style Guide를 참고하되, 프로젝트 특성에 맞게 조정한다.

---

## 1. 네이밍 규칙

### 파일명
```
PascalCase.h / PascalCase.cpp
예: Sample.h, OrderController.cpp, JsonStorage.h
```

### 클래스 / 구조체 / Enum
```cpp
// PascalCase
class OrderController { ... };
struct ProductionJob { ... };
enum class OrderStatus { RESERVED, CONFIRMED, PRODUCING, RELEASED, REJECTED };
```

### 함수 / 메서드
```cpp
// camelCase
void createOrder();
int calculateActualProduction(int shortage, double yieldRate);
bool isStockSufficient(const std::string& sampleId, int quantity);
```

### 변수 (지역, 매개변수)
```cpp
// camelCase
int orderQuantity = 200;
double yieldRate = 0.92;
std::string customerId;
```

### 멤버 변수
```cpp
// camelCase, 접두사 없음 (m_ 접두사 사용 금지)
class Sample {
private:
    std::string sampleId;
    std::string name;
    double avgProductionTime;
    double yieldRate;
    int stock;
};
```

### 상수 / Enum 값
```cpp
// UPPER_SNAKE_CASE
const int MAX_QUEUE_SIZE = 100;
const std::string DATA_DIR = "data/";

enum class OrderStatus {
    RESERVED,
    REJECTED,
    PRODUCING,
    CONFIRMED,
    RELEASED
};
```

### 인터페이스
```cpp
// 'I' 접두사 + PascalCase (PoC 패턴 계승)
class IModel { ... };
class IView { ... };
class IController { ... };
```

---

## 2. 파일 구조

### 헤더 파일 (.h)
```cpp
#pragma once

#include <string>
#include <vector>

class Sample {
public:
    Sample(const std::string& id, const std::string& name,
           double avgProductionTime, double yieldRate, int stock);

    // Getter
    std::string getSampleId() const;
    std::string getName() const;
    double getAvgProductionTime() const;
    double getYieldRate() const;
    int getStock() const;

    // Setter
    void setStock(int stock);

private:
    std::string sampleId;
    std::string name;
    double avgProductionTime;
    double yieldRate;
    int stock;
};
```

### 구현 파일 (.cpp)
```cpp
#include "Sample.h"

Sample::Sample(const std::string& id, const std::string& name,
               double avgProductionTime, double yieldRate, int stock)
    : sampleId(id), name(name),
      avgProductionTime(avgProductionTime),
      yieldRate(yieldRate),
      stock(stock) {}

std::string Sample::getSampleId() const { return sampleId; }
```

---

## 3. 들여쓰기 및 포맷

- **들여쓰기**: 공백 4칸 (탭 금지)
- **중괄호**: K&R 스타일 (여는 중괄호 같은 줄)
- **한 줄 최대 길이**: 100자

```cpp
// 올바른 예
void OrderController::approveOrder(const std::string& orderId) {
    auto order = orderRepository.findById(orderId);
    if (order.getStatus() != OrderStatus::RESERVED) {
        view.showError("RESERVED 상태의 주문만 승인 가능합니다.");
        return;
    }
    // ...
}

// 잘못된 예 — 중괄호 다음 줄
void approveOrder(const std::string& orderId)
{
    ...
}
```

---

## 4. 클래스 설계

### 멤버 선언 순서
```cpp
class SampleController {
public:
    // 생성자 / 소멸자
    explicit SampleController(SampleRepository& repo, SampleView& view);
    ~SampleController() = default;

    // public 메서드
    void registerSample();
    void listSamples();
    void searchSample();

private:
    // private 메서드
    bool validateSampleId(const std::string& id);

    // 멤버 변수 (마지막)
    SampleRepository& repo;
    SampleView& view;
};
```

### 의존성 주입
```cpp
// 참조(reference) 또는 포인터로 의존성 주입 — 소유권은 외부에서 관리
class OrderController {
public:
    explicit OrderController(OrderRepository& orderRepo,
                             SampleRepository& sampleRepo,
                             OrderView& view);
private:
    OrderRepository& orderRepo;
    SampleRepository& sampleRepo;
    OrderView& view;
};
```

---

## 5. 주석

```cpp
// 한 줄 주석: 코드의 WHY가 명확하지 않을 때만 작성
// 예: 수율 오차 10% 감안하여 0.9 곱함 (기획 요구사항)
int actualProduction = static_cast<int>(
    std::ceil(shortage / (yieldRate * 0.9))
);

// 함수/클래스 설명 주석은 헤더 파일에 한 줄로 작성
// WHAT 설명은 금지, WHY·제약사항만 기록
```

- `/* */` 블록 주석 사용 금지 (단, Doxygen 생성 제외)
- TODO는 `// TODO: <내용>` 형식으로 작성

---

## 6. #include 순서

```cpp
// 1. 연관 헤더 (현재 파일의 .h)
#include "OrderController.h"

// 2. 프로젝트 내부 헤더
#include "../model/Order.h"
#include "../repository/OrderRepository.h"
#include "../view/OrderView.h"

// 3. C++ 표준 라이브러리
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <filesystem>
```

---

## 7. 에러 처리

```cpp
// 예외 대신 반환값 + 콘솔 메시지 방식 사용 (PoC 패턴 계승)
bool SampleRepository::save(const Sample& sample) {
    if (sample.getSampleId().empty()) {
        return false;
    }
    // ...
    return true;
}

// View에서 에러 메시지 출력
void OrderView::showError(const std::string& message) {
    std::cout << "[오류] " << message << std::endl;
}
```

---

## 8. 데이터 영속성 (JsonStorage 패턴)

```cpp
// DataPersistence PoC의 JsonStorage 패턴을 그대로 사용
// 변경 발생 시 즉시 파일에 저장 (지연 저장 금지)
void OrderRepository::save(const Order& order) {
    orders.push_back(order);
    storage.persist(orders);   // 즉시 파일 반영
}

// 저장 경로는 상수로 관리
const std::string ORDERS_FILE = "data/orders.json";
const std::string SAMPLES_FILE = "data/samples.json";
const std::string JOBS_FILE    = "data/production_jobs.json";
```

---

## 9. 금지 사항

| 금지 항목 | 이유 |
|-----------|------|
| `using namespace std;` 전역 사용 | 네임스페이스 오염 |
| 원시 포인터 소유권 관리 | 메모리 누수 위험 — `std::unique_ptr` / `std::shared_ptr` 사용 |
| `goto` 문 | 흐름 추적 불가 |
| 매직 넘버 | 상수(`const`)로 이름 부여 |
| `m_`, `g_`, `s_` 접두사 | 불필요한 헝가리안 표기법 |
| 외부 라이브러리 추가 | PoC 방침 — 자체 구현 사용 |
