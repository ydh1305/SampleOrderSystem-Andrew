# API 설계 — 레이어 간 인터페이스 명세

콘솔 애플리케이션이므로 외부 HTTP API가 아닌 **레이어 간 내부 인터페이스(C++ 클래스 API)**를 정의한다.

---

## 1. 공용 인터페이스 (include/)

### IController
```cpp
// include/IController.h
class IController {
public:
    virtual ~IController() = default;
    virtual void run() = 0;
};
```

### IView
```cpp
// include/IView.h
class IView {
public:
    virtual ~IView() = default;
    virtual void render() = 0;
};
```

### IModel
```cpp
// include/IModel.h
class IModel {
public:
    virtual ~IModel() = default;
};
```

### ISampleRepository
```cpp
// include/ISampleRepository.h
class ISampleRepository {
public:
    virtual ~ISampleRepository() = default;
    virtual std::vector<Sample>   findAll() const = 0;
    virtual std::optional<Sample> findById(const std::string& sampleId) const = 0;
    virtual void                  save(const Sample& sample) = 0;
    virtual void                  update(const Sample& sample) = 0;
    virtual std::string           generateNextId() const = 0;
};
```

### IOrderRepository
```cpp
// include/IOrderRepository.h
class IOrderRepository {
public:
    virtual ~IOrderRepository() = default;
    virtual std::vector<Order>   findAll() const = 0;
    virtual std::optional<Order> findById(const std::string& orderId) const = 0;
    virtual std::vector<Order>   findByStatus(OrderStatus status) const = 0;
    virtual void                 save(const Order& order) = 0;
    virtual void                 update(const Order& order) = 0;
    virtual int                  getNextSequence(const std::string& date) const = 0;
};
```

### IProductionJobRepository
```cpp
// include/IProductionJobRepository.h
class IProductionJobRepository {
public:
    virtual ~IProductionJobRepository() = default;
    virtual std::vector<ProductionJob>   findAll() const = 0;
    virtual std::optional<ProductionJob> findById(const std::string& jobId) const = 0;
    virtual std::vector<ProductionJob>   findByStatus(JobStatus status) const = 0;
    virtual std::optional<ProductionJob> findRunning() const = 0;
    virtual std::vector<ProductionJob>   findWaiting() const = 0;
    virtual void                         save(const ProductionJob& job) = 0;
    virtual void                         update(const ProductionJob& job) = 0;
    virtual std::string                  generateNextId() const = 0;
};
```

---

## 2. Controller API

### MainController
```cpp
class MainController : public IController {
public:
    MainController(ISampleRepository&, IOrderRepository&, IProductionJobRepository&);
    void run() override;   // 메인 메뉴 루프 진입 (0 입력 시 종료)
};
```

### SampleController
```cpp
class SampleController : public IController {
public:
    explicit SampleController(ISampleRepository&);
    void run() override;   // 시료 관리 서브 메뉴 루프
};
```

### OrderController
```cpp
class OrderController : public IController {
public:
    OrderController(ISampleRepository&, IOrderRepository&, IProductionJobRepository&);
    void run() override;   // 주문 접수 + 승인/거절 서브 메뉴 루프
};
```

### ProductionController
```cpp
class ProductionController : public IController {
public:
    ProductionController(ISampleRepository&, IOrderRepository&, IProductionJobRepository&);
    void run() override;   // 생산라인 현황 출력 + 완료 처리
};
```

### MonitoringController
```cpp
class MonitoringController : public IController {
public:
    MonitoringController(ISampleRepository&, IOrderRepository&);
    void run() override;   // 모니터링 서브 메뉴 루프
};
```

### ReleaseController
```cpp
class ReleaseController : public IController {
public:
    ReleaseController(ISampleRepository&, IOrderRepository&);
    void run() override;   // 출고 처리 화면
};
```

---

## 3. Model API

### ProductionCalculator
```cpp
struct ProductionCalcResult {
    int    shortage;      // 부족분 (ea)
    int    actualProd;    // 실 생산량 (ea)
    double totalTime;     // 총 생산시간 (min)
};

class ProductionCalculator {
public:
    // 순수 함수 — 상태 없음
    static ProductionCalcResult calculate(
        int    quantity,
        int    stock,
        double yieldRate,
        double avgProductionTime
    );
    // 공식: actualProd = ceil(shortage / (yieldRate * 0.9))
    //       totalTime  = avgProductionTime * actualProd
};
```

### ProductionQueue
```cpp
class ProductionQueue {
public:
    explicit ProductionQueue(IProductionJobRepository&);

    void enqueue(ProductionJob& job);
    // job을 WAITING으로 저장. RUNNING 없으면 즉시 RUNNING 전환

    std::optional<ProductionJob> currentRunning() const;
    // 현재 RUNNING 작업 반환. 없으면 nullopt

    std::vector<ProductionJob> waitingList() const;
    // WAITING 목록 (enqueuedAt 오름차순)

    ProductionJob completeCurrentJob();
    // RUNNING → DONE. 다음 WAITING → RUNNING 자동 시작. 완료된 job 반환

    bool isEmpty() const;
    // RUNNING, WAITING 모두 없으면 true
};
```

### OrderIdGenerator
```cpp
class OrderIdGenerator {
public:
    // "ORD-20260612-0003" 형식 생성
    static std::string generate(const std::string& date, int seq);
    // date: "YYYYMMDD", seq: 1-based 정수 (4자리 zero-padding)
};
```

### 직렬화 인터페이스 (각 모델 공통)
```cpp
// 각 모델(Sample, Order, ProductionJob)이 구현하는 패턴
JsonValue toJson() const;
static T  fromJson(const JsonValue& json);
```

---

## 4. Repository API

> 각 Repository는 `include/` 의 인터페이스를 구현한다.  
> Controller는 인터페이스에만 의존 → 단위 테스트 시 gmock Mock으로 교체 가능.

### SampleRepository
```cpp
class SampleRepository : public ISampleRepository {
public:
    explicit SampleRepository(const std::string& filePath = "data/samples.json");

    std::vector<Sample>       findAll()    const override;
    std::optional<Sample>     findById(const std::string& sampleId) const override;
    void                      save(const Sample& sample) override;    // 중복 ID → 예외
    void                      update(const Sample& sample) override;  // 미존재 → 예외
    std::string               generateNextId() const override;        // "S-001", "S-002", ...
};
```

### OrderRepository
```cpp
class OrderRepository : public IOrderRepository {
public:
    explicit OrderRepository(const std::string& filePath = "data/orders.json");

    std::vector<Order>        findAll()    const override;
    std::optional<Order>      findById(const std::string& orderId) const override;
    std::vector<Order>        findByStatus(OrderStatus status) const override;
    void                      save(const Order& order) override;
    void                      update(const Order& order) override;
    int                       getNextSequence(const std::string& date) const override;
    // date: "YYYYMMDD" — 해당 날짜 마지막 순번+1 반환 (없으면 1)
};
```

### ProductionJobRepository
```cpp
class ProductionJobRepository : public IProductionJobRepository {
public:
    explicit ProductionJobRepository(const std::string& filePath = "data/production_jobs.json");

    std::vector<ProductionJob>    findAll()    const override;
    std::optional<ProductionJob>  findById(const std::string& jobId) const override;
    std::vector<ProductionJob>    findByStatus(JobStatus status) const override;
    std::optional<ProductionJob>  findRunning() const override;  // RUNNING 단건
    std::vector<ProductionJob>    findWaiting() const override;  // WAITING 목록 (enqueuedAt 오름차순)
    void                          save(const ProductionJob& job) override;
    void                          update(const ProductionJob& job) override;
    std::string                   generateNextId() const override;  // "JOB-YYYYMMDD-NNNN"
};
```

---

## 5. Storage API

### JsonValue
```cpp
class JsonValue {
public:
    // 생성
    JsonValue();                                           // null
    explicit JsonValue(bool);
    explicit JsonValue(int);
    explicit JsonValue(double);
    explicit JsonValue(const std::string&);
    explicit JsonValue(std::vector<JsonValue>);            // array
    explicit JsonValue(std::map<std::string, JsonValue>);  // object

    // 타입 확인
    JsonType type() const;

    // 값 접근 (타입 불일치 시 예외)
    bool        asBool()   const;
    int         asInt()    const;
    double      asDouble() const;
    const std::string&                          asString() const;
    const std::vector<JsonValue>&               asArray()  const;
    const std::map<std::string, JsonValue>&     asObject() const;

    // object 키 접근
    JsonValue&       operator[](const std::string& key);
    const JsonValue& operator[](const std::string& key) const;
    bool             contains(const std::string& key) const;

    // 직렬화
    std::string serialize(int indent = 2) const;
};
```

### JsonParser
```cpp
class JsonParser {
public:
    // JSON 문자열 → JsonValue 파싱. 실패 시 예외
    static JsonValue parse(const std::string& input);
};
```

### JsonStorage
```cpp
class JsonStorage {
public:
    explicit JsonStorage(const std::string& filePath);

    std::vector<JsonValue> loadAll();
    // 파일 없으면 빈 벡터 반환 (예외 없음)

    void saveAll(const std::vector<JsonValue>& items);
    // pretty-print로 즉시 파일 저장. data/ 디렉터리 없으면 자동 생성
};
```

---

## 6. 레이어 간 호출 흐름 예시

### 주문 승인 (재고 부족) 흐름
```
OrderController::approveOrders()                  // ISampleRepository&, IOrderRepository&, IProductionJobRepository& 사용
  │
  ├── orderRepo_.findByStatus(RESERVED)            // IOrderRepository API
  ├── view_.printReservedList(orders)              // View API
  ├── sampleRepo_.findById(order.sampleId)         // ISampleRepository API
  │
  ├── ProductionCalculator::calculate(...)         // Model API (순수 함수)
  │
  ├── ProductionJob job = { ... }
  ├── queue_.enqueue(job)                          // Model API (IProductionJobRepository 내부 사용)
  │     └── jobRepo_.save(job)                    // IProductionJobRepository API (내부)
  │
  ├── order.status = PRODUCING
  ├── orderRepo_.update(order)                     // IOrderRepository API
  │
  └── view_.printApprovalResult(order, &result)    // View API
```

---

## 7. Enum 정의

### OrderStatus
```cpp
enum class OrderStatus {
    RESERVED,   // 주문 접수
    REJECTED,   // 주문 거절
    PRODUCING,  // 생산 중 (재고 부족)
    CONFIRMED,  // 출고 대기 (재고 충분 또는 생산 완료)
    RELEASED    // 출고 완료
};

std::string   toString(OrderStatus status);
OrderStatus   orderStatusFromString(const std::string& s);
```

### JobStatus
```cpp
enum class JobStatus {
    WAITING,   // 생산 대기
    RUNNING,   // 생산 중
    DONE       // 생산 완료
};

std::string   toString(JobStatus status);
JobStatus     jobStatusFromString(const std::string& s);
```
