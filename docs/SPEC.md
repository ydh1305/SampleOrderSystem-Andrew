# SPEC - 반도체 시료 생산주문관리 시스템 기술 명세

## 1. 시스템 아키텍처

### 1.1 패턴
**MVC (Model-View-Controller)** 패턴 적용

```
src/
├── model/       # 데이터 모델 및 비즈니스 로직
├── view/        # 콘솔 출력 및 사용자 입력 처리
├── controller/  # 요청 처리 및 흐름 제어
└── repository/  # 데이터 영속성 계층
```

### 1.2 실행 방식
- 콘솔(CLI) 기반 애플리케이션
- 단일 프로세스, 싱글 스레드 실행
- 메인 루프에서 메뉴 선택 → 기능 실행 → 결과 출력 반복

---

## 2. 데이터 모델

### 2.1 Sample (시료)

| 필드 | 타입 | 설명 |
|------|------|------|
| sampleId | String | 고유 식별자 (예: `S-001`) |
| name | String | 시료명 |
| avgProductionTime | double | 평균 생산시간 (min/ea) |
| yieldRate | double | 수율 (0.0 ~ 1.0) |
| stock | int | 현재 재고 수량 (ea) |

### 2.2 Order (주문)

| 필드 | 타입 | 설명 |
|------|------|------|
| orderId | String | 주문번호 (예: `ORD-20260416-0001`) |
| sampleId | String | 대상 시료 ID (FK) |
| customerName | String | 고객명 |
| quantity | int | 주문 수량 |
| status | OrderStatus | 주문 상태 |
| createdAt | LocalDateTime | 주문 생성일시 |
| updatedAt | LocalDateTime | 상태 변경일시 |

### 2.3 OrderStatus (주문 상태 Enum)

```
RESERVED   → 주문 접수
REJECTED   → 주문 거절
PRODUCING  → 생산 중 (재고 부족)
CONFIRMED  → 출고 대기 (재고 충분)
RELEASED   → 출고 완료
```

### 2.4 ProductionJob (생산 작업)

| 필드 | 타입 | 설명 |
|------|------|------|
| jobId | String | 생산 작업 ID |
| orderId | String | 연결된 주문 ID |
| sampleId | String | 생산할 시료 ID |
| shortage | int | 부족분 수량 |
| actualProd | int | 실 생산량 (`ceil(shortage / (yieldRate × 0.9))`) |
| totalTime | double | 총 생산 시간 (min) |
| status | JobStatus | WAITING / RUNNING / DONE |
| enqueuedAt | LocalDateTime | 큐 등록 시각 |

---

## 3. 핵심 비즈니스 로직

### 3.1 주문 승인 처리 흐름

```
승인 요청 수신
    │
    ▼
현재 재고 확인
    │
    ├─ 재고 >= 주문 수량 ──▶ 재고 차감 + 상태 → CONFIRMED
    │
    └─ 재고 < 주문 수량  ──▶ 부족분 계산 + ProductionJob 생성
                              + 생산 큐(FIFO) 등록
                              + 상태 → PRODUCING
```

### 3.2 생산량 계산 공식

```
부족분       = 주문 수량 - 현재 재고
실 생산량    = ceil(부족분 / (수율 × 0.9))
총 생산시간  = 평균 생산시간(min/ea) × 실 생산량
```

예시: 부족분 170ea, 수율 0.92
- 실 생산량 = ceil(170 / (0.92 × 0.9)) = ceil(170 / 0.828) = ceil(205.3) = **206 ea**
- 수율 0.92 기준 정상품 ≈ 206 × 0.92 = 189.5 → 189ea 획득 (170 이상 충족)

### 3.3 생산 완료 처리

```
생산 완료 이벤트
    │
    ▼
생산된 정상 시료를 재고에 추가
    │
    ▼
해당 주문 상태: PRODUCING → CONFIRMED
```

### 3.4 재고 상태 판단 기준

| 상태 | 조건 |
|------|------|
| 고갈 | 재고 == 0 |
| 부족 | 재고 > 0 이나 RESERVED/CONFIRMED 합산 주문량 대비 부족 |
| 여유 | 주문 대비 재고 충분 |

---

## 4. 생산라인 스케줄링

- **방식**: FIFO (Queue 자료구조)
- **단일 라인**: 한 번에 하나의 생산 작업만 처리
- 생산 완료 후 다음 대기 작업 자동 시작
- 주문 승인 시 재고 부족이면 즉시 큐에 추가

---

## 5. 주문번호 생성 규칙

```
형식: ORD-{YYYYMMDD}-{순번 4자리}
예시: ORD-20260612-0001
```

- 시스템 전체에서 순번은 단조 증가 (재시작 후에도 유지)

---

## 6. 데이터 영속성

- 애플리케이션 종료 후 재실행 시에도 모든 데이터 유지
- 저장 방식: JSON 파일 / SQLite / 관계형 DB 중 선택 (팀/개인 결정)
- CRUD 작업은 Repository 계층을 통해서만 수행
- 최소 저장 대상: Sample, Order, ProductionJob

---

## 7. 콘솔 UI 규칙

- 메뉴 선택: 숫자 입력 방식
- 입력 프롬프트: `선택 > `
- 상태값은 대문자 표기 (RESERVED, CONFIRMED 등)
- 뒤로가기: `[0] 위로` 또는 `[0] 뒤로`
- 확인/취소: `[Y] 확인 / [N] 취소`
- 목록 표시 시 컬럼 헤더 포함

---

## 8. 패키지 구조 (C++ 기준)

> 전체 디렉터리 구조는 `docs/architecture.md` 3절을 참조한다.

핵심 구성만 요약하면 다음과 같다:

```
SampleOrderSystem/
├── main.cpp
├── include/                    ← 공용 인터페이스 헤더
│   ├── IController.h
│   ├── IView.h
│   ├── IModel.h
│   ├── ISampleRepository.h
│   ├── IOrderRepository.h
│   └── IProductionJobRepository.h
└── src/
    ├── model/                  ← 도메인 엔티티 + 비즈니스 로직
    │   ├── Sample.h/.cpp
    │   ├── Order.h/.cpp
    │   ├── ProductionJob.h/.cpp
    │   ├── OrderStatus.h
    │   ├── JobStatus.h
    │   └── ProductionCalculator.h/.cpp
    ├── view/                   ← 콘솔 I/O
    │   ├── ConsoleUtil.h/.cpp
    │   ├── MainView.h/.cpp
    │   ├── SampleView.h/.cpp
    │   ├── OrderView.h/.cpp
    │   ├── ProductionView.h/.cpp
    │   ├── MonitoringView.h/.cpp
    │   └── ReleaseView.h/.cpp
    ├── controller/             ← 흐름 제어
    │   ├── MainController.h/.cpp
    │   ├── SampleController.h/.cpp
    │   ├── OrderController.h/.cpp
    │   ├── ProductionController.h/.cpp
    │   ├── MonitoringController.h/.cpp
    │   └── ReleaseController.h/.cpp
    └── repository/             ← 데이터 접근
        ├── SampleRepository.h/.cpp
        ├── OrderRepository.h/.cpp
        └── ProductionJobRepository.h/.cpp
```

---

## 9. 테스트 요구사항

- 단위 테스트: 핵심 비즈니스 로직 (생산량 계산, 상태 전이, 재고 판단 등)
- 통합 테스트: 주문 승인 → 재고 차감 → 상태 변경 흐름
- Dummy 데이터 생성 도구를 통해 초기 데이터 세팅 가능

---

## 10. PoC Repository 명명 규칙

| PoC 항목 | Repository 이름 |
|----------|----------------|
| MVC 스켈레톤 코드 | `ConsoleMVC-본인영문이름-사번` |
| 데이터 영속성 처리 | `DataPersistence-본인영문이름-사번` |
| 데이터 모니터링 Tool | `DataMonitor-본인영문이름-사번` |
| Dummy 데이터 생성 Tool | `DummyDataGenerator-본인영문이름-사번` |
| 메인 프로젝트 | `SampleOrderSystem-본인영문이름-사번` |
