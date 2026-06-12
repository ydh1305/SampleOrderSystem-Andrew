# SampleOrderSystem

반도체 시료 생산주문관리 시스템 (S-Semi)

시료 등록 → 주문 접수 → 승인/거절 → 생산 → 출고 흐름을 관리하는 콘솔 기반 MVC 애플리케이션입니다.

---

## 기술 스택

| 항목 | 내용 |
|------|------|
| 언어 | C++17 |
| 빌드 환경 | Visual Studio 2022 (MSVC v143), Windows x64 |
| 테스트 프레임워크 | Google Mock (gmock) 1.11.0 — NuGet 패키지 |
| 데이터 저장 | JSON 파일 (자체 구현 파서, 외부 라이브러리 없음) |
| UI | 콘솔 기반 (ANSI VT100 색상 코드) |

---

## 프로젝트 구조

```
SampleOrderSystem/
├── include/                    # 공용 인터페이스 & 모델 재수출 헤더
│   ├── IController.h           # 공통 Controller 인터페이스 (run())
│   ├── IView.h                 # 공통 View 인터페이스 (render(), getInput())
│   ├── IModel.h                # 공통 Model 인터페이스
│   ├── ISampleRepository.h     # 시료 저장소 인터페이스
│   ├── IOrderRepository.h      # 주문 저장소 인터페이스
│   ├── IProductionJobRepository.h  # 생산작업 저장소 인터페이스
│   ├── Sample.h / Order.h / ProductionJob.h   # 모델 재수출
│   ├── OrderStatus.h / JobStatus.h            # Enum 재수출
│   └── ...
│
├── src/
│   ├── model/                  # 도메인 엔티티 & 비즈니스 유틸
│   │   ├── Sample.h/.cpp           # 시료 (sampleId, name, avgProductionTime, yieldRate, stock)
│   │   ├── Order.h/.cpp            # 주문 (orderId, sampleId, customerName, quantity, status, ...)
│   │   ├── ProductionJob.h/.cpp    # 생산작업 (jobId, orderId, shortage, actualProd, totalTime, ...)
│   │   ├── OrderStatus.h           # enum RESERVED·REJECTED·PRODUCING·CONFIRMED·RELEASED
│   │   ├── JobStatus.h             # enum WAITING·RUNNING·DONE
│   │   ├── ProductionCalculator.h/.cpp   # 생산량 계산 순수 함수
│   │   ├── OrderIdGenerator.h/.cpp      # 주문번호 생성 순수 함수
│   │   └── ProductionQueue.h/.cpp       # FIFO 생산 큐 (파일 기반 영속)
│   │
│   ├── repository/             # JSON 파일 기반 CRUD
│   │   ├── SampleRepository.h/.cpp         (ISampleRepository 구현)
│   │   ├── OrderRepository.h/.cpp          (IOrderRepository 구현)
│   │   └── ProductionJobRepository.h/.cpp  (IProductionJobRepository 구현)
│   │
│   ├── service/                # 복합 비즈니스 로직
│   │   ├── OrderApprovalService.h/.cpp     # 주문 승인·거절 (재고 분기, 생산 큐 등록)
│   │   └── ProductionService.h/.cpp        # 생산 완료 처리 (재고 증가, 상태 전이)
│   │
│   ├── controller/             # 사용자 요청 흐름 제어
│   │   ├── MainController.h/.cpp       # 메인 메뉴 루프
│   │   ├── SampleController.h/.cpp     # 시료 등록·조회·검색
│   │   ├── OrderController.h/.cpp      # 주문 접수·승인/거절
│   │   ├── MonitoringController.h/.cpp # 주문 현황·재고 모니터링
│   │   ├── ProductionController.h/.cpp # 생산라인 조회·완료 처리
│   │   └── ReleaseController.h/.cpp    # 출고 처리
│   │
│   ├── view/                   # 콘솔 I/O
│   │   ├── ConsoleUtil.h/.cpp      # ANSI 색상, 날짜/시간, 입력 유틸
│   │   ├── MainView.h/.cpp
│   │   ├── SampleView.h/.cpp
│   │   ├── OrderView.h/.cpp
│   │   ├── MonitoringView.h/.cpp
│   │   ├── ProductionView.h/.cpp
│   │   └── ReleaseView.h/.cpp
│   │
│   └── storage/                # JSON 파일 I/O
│       ├── JsonValue.h/.cpp    # null·bool·int·double·string·array·object 지원
│       ├── JsonParser.h/.cpp   # 재귀 하강 파서 (외부 라이브러리 없음)
│       └── JsonStorage.h/.cpp  # 파일 Read/Write, 변경 즉시 persist()
│
└── main.cpp                    # 진입점

SampleOrderSystemTests/
├── main_test.cpp               # RUN_ALL_TESTS() 진입점
├── mocks/
│   └── MockRepositories.h      # MockSampleRepository, MockOrderRepository,
│                               #   MockProductionJobRepository (gmock MOCK_METHOD)
├── test_json_parser.cpp        # JsonParser 단위 테스트
├── test_json_storage.cpp       # JsonStorage 단위 테스트
├── test_console_util.cpp       # ConsoleUtil 단위 테스트
├── test_models.cpp             # Sample, Order, ProductionJob 직렬화 테스트
├── test_sample_repository.cpp  # SampleRepository 단위 테스트
├── test_order_repository.cpp   # OrderRepository 단위 테스트
├── test_production_job_repository.cpp  # ProductionJobRepository 단위 테스트
├── test_production_calculator.cpp      # ProductionCalculator 단위 테스트
├── test_model_utils.cpp        # OrderIdGenerator, ProductionQueue 단위 테스트
├── test_order_approval_service.cpp     # OrderApprovalService 단위 테스트
├── test_production_service.cpp         # ProductionService 단위 테스트
├── test_views.cpp              # View 레이어 단위 테스트
└── test_controllers.cpp        # Controller 레이어 단위 테스트 (Mock 주입)

data/                           # 런타임 생성 JSON 파일 (.gitignore)
├── samples.json
├── orders.json
└── production_jobs.json
```

---

## 아키텍처

```
[ 사용자 입력 ]
      ↓
 Controller ──→ View (콘솔 출력)
      ↓
  Service (복합 비즈니스 로직)
      ↓
 Repository (IXxxRepository 인터페이스)
      ↓
 JsonStorage (파일 I/O)
      ↓
  data/*.json
```

**의존성 방향**: Controller → Service → IRepository ← Repository (구체 구현)

Controller와 Service는 **인터페이스(IXxxRepository)**에만 의존하므로, 단위 테스트 시 Mock으로 교체 가능합니다.

---

## 메인 메뉴

```
========================================
  S-Semi 시료 생산주문관리 시스템
  2026-06-12 15:30:00
  시료: 4종 | 재고: 480ea | 주문: 12건 | 생산대기: 2건
========================================
[1] 시료 관리
[2] 시료 주문
[3] 주문 승인/거절
[4] 모니터링
[5] 생산라인 조회
[6] 출고 처리
[0] 종료
```

---

## 주문 상태 흐름

```
접수
 ↓
RESERVED ──(승인 + 재고 충분)──→ CONFIRMED ──→ RELEASED
         ↘
          (승인 + 재고 부족) ──→ PRODUCING ──→ CONFIRMED ──→ RELEASED
         ↘
          (거절) ──→ REJECTED
```

---

## 핵심 비즈니스 로직

### 생산량 계산 (`ProductionCalculator`)

```
부족분      = 주문 수량 - 현재 재고
실 생산량   = ceil(부족분 / (수율 × 0.9))
총 생산시간 = 평균 생산시간(min/ea) × 실 생산량
```

예시: 부족분 170, 수율 0.92 → `ceil(170 / 0.828)` = **206 ea**

### 생산 완료 시 재고 증가 (`ProductionService`)

```
증가 재고 = floor(실 생산량 × 수율)
```

### 주문번호 생성 (`OrderIdGenerator`)

```
형식: ORD-YYYYMMDD-NNNN
예시: ORD-20260612-0003  (2026-06-12의 3번째 주문)
```
날짜가 바뀌면 순번이 0001부터 재시작됩니다.

### 생산 큐 (`ProductionQueue`)

FIFO 방식. `ProductionJobRepository`를 통해 파일에 영속 저장되므로 애플리케이션 재시작 후에도 큐 상태가 유지됩니다.

---

## 데이터 저장

모든 데이터는 `data/` 디렉터리 아래 JSON 파일로 저장됩니다.

```json
// data/samples.json
[
  {
    "sampleId": "S-001",
    "name": "실리콘 웨이퍼",
    "avgProductionTime": 0.8,
    "yieldRate": 0.92,
    "stock": 480
  }
]
```

```json
// data/orders.json
[
  {
    "orderId": "ORD-20260612-0001",
    "sampleId": "S-001",
    "customerName": "고객사 A",
    "quantity": 100,
    "status": "CONFIRMED",
    "createdAt": "2026-06-12 09:00:00",
    "updatedAt": "2026-06-12 09:05:00"
  }
]
```

```json
// data/production_jobs.json
[
  {
    "jobId": "JOB-001",
    "orderId": "ORD-20260612-0002",
    "sampleId": "S-001",
    "shortage": 170,
    "actualProd": 206,
    "totalTime": 164.8,
    "status": "RUNNING",
    "enqueuedAt": "2026-06-12 09:10:00",
    "startedAt": "2026-06-12 09:10:00",
    "completedAt": ""
  }
]
```

---

## 빌드 방법

### 요구 환경

- Windows 10/11 (x64)
- Visual Studio 2022 (MSVC v143, C++17)
- NuGet 패키지 복원 (`packages/gmock.1.11.0/` 자동 설치)

### Visual Studio에서 빌드

1. `SampleOrderSystem.sln` 열기
2. 구성: **Debug / x64** 선택
3. **빌드 → 솔루션 빌드** (`Ctrl+Shift+B`)

### MSBuild CLI

```powershell
& "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" `
  SampleOrderSystem.sln /p:Configuration=Debug /p:Platform=x64
```

---

## 테스트 실행

```powershell
# 빌드 후 테스트 바이너리 실행
.\SampleOrderSystemTests\x64\Debug\SampleOrderSystemTests.exe

# 특정 테스트만 실행
.\SampleOrderSystemTests\x64\Debug\SampleOrderSystemTests.exe --gtest_filter="*ProductionCalculator*"
```

### 현재 테스트 현황 (master 기준)

| 테스트 파일 | 스위트 | 테스트 수 |
|-------------|--------|---------|
| test_json_parser.cpp | JsonParserTest | 12 |
| test_json_storage.cpp | JsonStorageTest | 4 |
| test_console_util.cpp | ConsoleUtilTest | 3 |
| test_models.cpp | SampleModelTest, OrderModelTest, ProductionJobModelTest, OrderStatusTest | 14 |
| test_sample_repository.cpp | SampleRepositoryTest | 6 |
| test_order_repository.cpp | OrderRepositoryTest | 7 |
| test_production_job_repository.cpp | ProductionJobRepositoryTest | 8 |
| test_production_calculator.cpp | ProductionCalculatorTest | 8 |
| test_model_utils.cpp | OrderIdGeneratorTest, ProductionQueueTest | 10 |
| test_order_approval_service.cpp | OrderApprovalServiceTest | 5 |
| test_production_service.cpp | ProductionServiceTest | 4 |
| test_views.cpp | 6종 ViewTest | 24 (feature) |
| test_controllers.cpp | 6종 ControllerTest | 13 (feature) |
| **합계** | **13+ 스위트** | **71+** |

테스트는 임시 JSON 파일(`data/test_*.json`)을 생성하고 TearDown에서 자동 삭제합니다.

---

## TDD 개발 원칙

모든 기능은 **RED → GREEN → REFACTOR** 사이클로 구현합니다.

```
RED    : 실패하는 테스트 먼저 작성
GREEN  : 테스트를 통과하는 최소 구현
REFACTOR: 중복 제거, 네이밍 개선 (테스트는 계속 GREEN)
```

Controller 계층 테스트는 `MockRepositories.h`의 Mock 객체를 주입하여 파일 I/O 없이 실행합니다.

```cpp
// 예시: SampleController 단위 테스트
TEST_F(SampleControllerTest, AddSampleCallsSave) {
    EXPECT_CALL(mockRepo_, generateNextId()).WillOnce(Return("S-001"));
    EXPECT_CALL(mockRepo_, save(_)).Times(1);
    ctrl_->addSample();
}
```

---

## 문서

| 문서 | 설명 |
|------|------|
| `docs/PRD.md` | 전체 요구사항 (기능 명세, 역할, 상태 흐름) |
| `docs/SPEC.md` | 기술 명세 (데이터 모델, 비즈니스 로직, 패키지 구조) |
| `docs/architecture.md` | 시스템 아키텍처 및 설계 결정 |
| `docs/api.md` | 클래스 인터페이스 API 레퍼런스 |
| `docs/database.md` | JSON 스키마 정의 |
| `docs/code-convention.md` | C++ 코딩 컨벤션 |
| `docs/commit-convention.md` | 커밋 메시지 규칙 |
| `docs/features/` | 기능별 상세 명세 (메뉴, 시료, 주문, 승인, 모니터링, 생산, 출고) |
| `docs/verification/` | Phase별 검증 결과 보고서 |
