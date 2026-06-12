# Architecture — 반도체 시료 생산주문관리 시스템

## 1. 시스템 개요

콘솔 기반 단일 프로세스 애플리케이션. 담당자가 직접 명령을 입력하여 시료 등록·주문·승인·생산·출고 흐름을 처리한다.

```
[사용자 입력] → [Controller] → [Model / Repository] → [JSON 파일]
                     ↓
               [View 출력]
```

---

## 2. 아키텍처 패턴: MVC

### 선택 이유
- PoC(ConsoleMVC-Andrew)에서 검증된 구조를 그대로 계승하여 학습 비용 최소화
- 콘솔 I/O와 비즈니스 로직의 혼재를 방지 → 단위 테스트 가능한 구조
- 인터페이스(`IController`, `IView`, `IModel`) 기반으로 레이어 교체 가능

### 레이어 구성

| 레이어 | 디렉터리 | 역할 |
|--------|----------|------|
| Controller | `src/controller/` | 사용자 요청 처리, 흐름 제어, 레이어 조율 |
| Model | `src/model/` | 도메인 엔티티, 비즈니스 로직 (생산량 계산, 상태 전이) |
| View | `src/view/` | 콘솔 출력, 사용자 입력 수집 |
| Repository | `src/repository/` | JSON 파일 기반 CRUD, 데이터 영속성 |
| Storage | `src/storage/` | JSON 파서 및 파일 I/O 원시 계층 |

### 의존성 방향 (단방향)

```
Controller
  ├── → Model           (비즈니스 로직 호출)
  ├── → View            (출력 지시)
  └── → IRepository     (인터페이스에만 의존 — 구체 클래스 직접 참조 금지)
          ↑
     SampleRepository    (ISampleRepository 구현)
     OrderRepository     (IOrderRepository 구현)
     ProductionJobRepository (IProductionJobRepository 구현)

Repository → Storage (JsonStorage)

View       → (입력만 받음, 비즈니스 로직 없음)
Model      → (순수 데이터 + 계산, 외부 의존 없음)
```

> **역방향 참조 금지**: View·Model·Repository가 Controller를 참조하지 않는다.  
> **인터페이스 의존**: Controller는 `ISampleRepository`·`IOrderRepository`·`IProductionJobRepository` 인터페이스에만 의존하므로, 단위 테스트 시 gmock Mock으로 교체 가능하다.

---

## 3. 디렉터리 구조

```
SampleOrderSystem-Andrew/
├── SampleOrderSystem/                ← 메인 애플리케이션 프로젝트
│   ├── main.cpp                      ← 진입점
│   ├── include/                      ← 공용 인터페이스 헤더
│   │   ├── IController.h
│   │   ├── IView.h
│   │   ├── IModel.h
│   │   ├── ISampleRepository.h
│   │   ├── IOrderRepository.h
│   │   └── IProductionJobRepository.h
│   └── src/
│       ├── model/                    ← 도메인 엔티티 + 비즈니스 로직
│       │   ├── Sample.h/.cpp
│       │   ├── Order.h/.cpp
│       │   ├── ProductionJob.h/.cpp
│       │   ├── OrderStatus.h
│       │   ├── JobStatus.h
│       │   ├── ProductionCalculator.h/.cpp
│       │   ├── ProductionQueue.h/.cpp
│       │   └── OrderIdGenerator.h/.cpp
│       ├── view/                     ← 콘솔 I/O
│       │   ├── ConsoleUtil.h/.cpp
│       │   ├── MainView.h/.cpp
│       │   ├── SampleView.h/.cpp
│       │   ├── OrderView.h/.cpp
│       │   ├── ProductionView.h/.cpp
│       │   ├── MonitoringView.h/.cpp
│       │   └── ReleaseView.h/.cpp
│       ├── controller/               ← 흐름 제어
│       │   ├── MainController.h/.cpp
│       │   ├── SampleController.h/.cpp
│       │   ├── OrderController.h/.cpp
│       │   ├── ProductionController.h/.cpp
│       │   ├── MonitoringController.h/.cpp
│       │   └── ReleaseController.h/.cpp
│       ├── repository/               ← 데이터 접근
│       │   ├── SampleRepository.h/.cpp
│       │   ├── OrderRepository.h/.cpp
│       │   └── ProductionJobRepository.h/.cpp
│       └── storage/                  ← JSON 파서 + 파일 I/O
│           ├── JsonValue.h/.cpp
│           ├── JsonParser.h/.cpp
│           └── JsonStorage.h/.cpp
├── SampleOrderSystemTests/           ← 테스트 프로젝트 (gmock 1.11.0)
│   ├── main_test.cpp                 ← RUN_ALL_TESTS() 진입점
│   ├── mocks/
│   │   └── MockRepositories.h        ← MockSampleRepository, MockOrderRepository, MockProductionJobRepository
│   ├── test_json_parser.cpp
│   ├── test_json_storage.cpp
│   ├── test_models.cpp
│   ├── test_sample_repository.cpp
│   ├── test_order_repository.cpp
│   ├── test_production_job_repository.cpp
│   ├── test_sample_management.cpp
│   ├── test_order_placement.cpp
│   ├── test_order_approval.cpp
│   ├── test_monitoring.cpp
│   ├── test_production_completion.cpp
│   ├── test_release_processing.cpp
│   └── test_integration.cpp
├── data/                             ← 런타임 JSON 저장소 (gitignore)
│   ├── samples.json
│   ├── orders.json
│   └── production_jobs.json
├── docs/                             ← 프로젝트 문서
└── temp_docs/                        ← 구현 계획 문서
```

---

## 4. 기술 스택 및 선택 이유

### 4.1 언어: C++17

| 이유 | 설명 |
|------|------|
| PoC 계승 | 4개 PoC 모두 C++17 기반, 동일 스택으로 통일 |
| `std::optional` | Repository의 `findById` 결과 표현에 활용 (nullopt = 미존재) |
| `std::filesystem` | `data/` 디렉터리 자동 생성에 활용 |
| 구조적 바인딩 | 가독성 높은 코드 작성 가능 |

### 4.2 빌드 환경: Visual Studio 2022 (MSVC v143), Windows x64

| 이유 | 설명 |
|------|------|
| 표준 환경 통일 | 팀/개인 과제 환경이 Windows이므로 플랫폼 이슈 최소화 |
| 디버거 | MSVC 디버거로 콘솔 앱 디버깅이 용이 |
| NuGet | gmock 패키지 관리를 NuGet으로 처리하여 설치 간소화 |

### 4.3 데이터 저장: 자체 구현 JSON 파서

| 이유 | 설명 |
|------|------|
| 외부 라이브러리 없음 | PoC 제약 조건 계승 — 빌드 환경 의존성 최소화 |
| 학습 목적 | JSON 파서 직접 구현으로 파싱 원리 이해 |
| 충분한 기능 | 본 프로젝트 수준의 단순 배열/오브젝트 저장에 외부 라이브러리 불필요 |
| DataPersistence-Andrew 검증 | PoC에서 이미 동일 패턴 동작 확인 완료 |

> **트레이드오프**: 성능·안정성은 nlohmann/json 같은 검증된 라이브러리에 미치지 못함. 그러나 PoC 규모에서는 문제 없음.

### 4.4 테스트 프레임워크: Google Mock (gmock) 1.11.0

| 이유 | 설명 |
|------|------|
| Mock 지원 | Repository 계층을 Mock으로 교체하여 Controller 단위 테스트 가능 |
| gtest 통합 | `TEST()`, `EXPECT_*`, `ASSERT_*` 매크로로 간결한 테스트 작성 |
| NuGet 설치 | Visual Studio에서 패키지 관리 일원화 |

### 4.5 UI: 콘솔 기반 (ANSI VT100)

| 이유 | 설명 |
|------|------|
| 요구사항 | PRD에서 콘솔 기반으로 명시 |
| 단순성 | GUI 없이 기능 로직에 집중 |
| ANSI 색상 | Windows 10+ `ENABLE_VIRTUAL_TERMINAL_PROCESSING`으로 색상 지원 |

---

## 5. 핵심 설계 결정

### 5.1 Repository 인메모리 캐시 없음
매 호출마다 JSON 파일을 읽는 방식. PoC 규모(수십~수백 건)에서 성능 문제 없고, 구현 단순성 우선.

### 5.2 ProductionQueue — 파일 기반 영속 큐
`std::queue`를 인메모리로 운영하지 않고 `ProductionJobRepository`를 통해 파일에서 WAITING/RUNNING 상태를 조회하는 방식. 재시작 후에도 큐 상태 유지.

### 5.3 단일 프로세스, 싱글 스레드
동시성 처리 없음. 메뉴 선택 → 처리 → 결과 출력의 단순 루프. 경쟁 조건(race condition) 고려 불필요.

### 5.4 생산 완료 수동 처리
실제 설비 연동 없이 담당자가 `[C]` 입력으로 완료 처리. 타이머/백그라운드 스레드 없음.

---

## 6. 주문 상태 전이 다이어그램

```
                    [주문 접수]
                        │
                    RESERVED
                        │
           ┌────────────┴────────────┐
        [승인]                    [거절]
           │                         │
    재고 확인                     REJECTED
    ┌───────┴──────┐
  충분             부족
    │               │
CONFIRMED       PRODUCING
    │               │
    │          [생산 완료]
    │               │
    └───────┬───────┘
            │
        CONFIRMED
            │
        [출고 처리]
            │
        RELEASED
```
