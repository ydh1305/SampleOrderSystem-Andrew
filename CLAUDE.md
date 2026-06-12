# CLAUDE.md - 반도체 시료 생산주문관리 시스템

## 프로젝트 개요

S-Semi 사의 반도체 시료 생산주문관리 시스템 (SampleOrderSystem).
콘솔 기반 MVC 애플리케이션으로, 시료 등록·주문 접수·승인·생산·출고 흐름을 관리한다.

## 핵심 문서

- **`docs/PRD.md`** — 전체 요구사항 (기능 명세, 역할, 주문 상태, 비기능 요구사항)
- **`docs/SPEC.md`** — 기술 명세 (데이터 모델, 비즈니스 로직, 패키지 구조, 생산량 공식)
- **`docs/commit-convention.md`** — 커밋 메시지 규칙 (Conventional Commits 기반, 프로젝트 scope 목록 포함)
- **`docs/code-convention.md`** — C++ 코딩 컨벤션 (네이밍, 파일 구조, 포맷, 금지 사항)

### 기능 명세 (docs/features/)
- **`1.main-menu.md`** — 메인 메뉴 (시스템 현황 요약, 메뉴 진입, 입력 유효성)
- **`2.sample-management.md`** — 시료 관리 (등록, 목록 조회, 검색)
- **`3.order-placement.md`** — 시료 주문 접수 (RESERVED 생성, 주문번호 생성 규칙)
- **`4.order-approval.md`** — 주문 승인/거절 (재고 분기, CONFIRMED/PRODUCING/REJECTED 전이)
- **`5.monitoring.md`** — 모니터링 (상태별 주문량, 시료별 재고 상태)
- **`6.production-line.md`** — 생산라인 (FIFO 큐, 생산량 공식, 완료 처리)
- **`7.release-processing.md`** — 출고 처리 (CONFIRMED → RELEASED 전이)

## 아키텍처

MVC 패턴 적용:
- `model/` — 데이터 모델 (Sample, Order, ProductionJob) 및 비즈니스 로직
- `view/` — 콘솔 출력 및 사용자 입력 처리
- `controller/` — 요청 처리 및 흐름 제어
- `repository/` — 데이터 영속성 계층 (CRUD)

## 주문 상태 흐름

```
RESERVED → (승인 + 재고 충분) → CONFIRMED → RELEASED
RESERVED → (승인 + 재고 부족) → PRODUCING → CONFIRMED → RELEASED
RESERVED → (거절) → REJECTED
```

## 핵심 공식

```
실 생산량   = ceil(부족분 / (수율 × 0.9))
총 생산시간 = 평균 생산시간(min/ea) × 실 생산량
부족분      = 주문 수량 - 현재 재고
```

## 기술 스택 (PoC 기반)

모든 PoC가 동일한 스택을 사용하며, 본 프로젝트도 이를 따른다.

- **언어**: C++17
- **빌드 환경**: Visual Studio 2022 (MSVC v143), Windows x64
- **데이터 저장**: JSON 파일 (자체 구현 JSON 파서, 외부 라이브러리 없음)
- **UI**: 콘솔 기반 (ANSI VT100 색상 코드 지원)
- **테스트 프레임워크**: Google Mock (gmock) 1.11.0 — NuGet 패키지로 설치
  - 패키지 경로: `packages/gmock.1.11.0/`
  - 테스트 프로젝트에서 `gmock.targets` import하여 사용
  - Mock 객체 및 EXPECT_*/ASSERT_* 매크로로 단위 테스트 작성

---

## PoC 레포지토리 참조

### 1. MVC 스켈레톤 — [ConsoleMVC-Andrew](https://github.com/ydh1305/ConsoleMVC-Andrew)

MVC 레이어 분리 구조의 기반. 본 프로젝트의 패키지 구조·인터페이스 설계를 그대로 계승한다.

**디렉토리 구조**
```
Models/       ← 도메인 엔티티 + IModel 인터페이스
Views/        ← 콘솔 I/O + IView 인터페이스
Controllers/  ← CRUD & 메뉴 조율 + IController 인터페이스
main.cpp      ← 진입점
```

**핵심 설계 원칙**
- 인터페이스 기반 (`IModel`, `IView`, `IController`) → 각 레이어 독립 교체 가능
- 단방향 의존성: Controller → Model/View (역방향 참조 금지)
- 데이터는 Controller를 통해서만 Model에 접근

---

### 2. 데이터 영속성 — [DataPersistence-Andrew](https://github.com/ydh1305/DataPersistence-Andrew)

JSON 파일 기반 영속성 계층의 기반. 본 프로젝트의 Repository 계층 구현 시 참조한다.

**핵심 클래스**
- `JsonStorage` — JSON 파일 I/O 및 자체 파서 (외부 라이브러리 없음)
- `ItemRepository` — CRUD 비즈니스 로직
- `Item` — 데이터 모델 (id, name, description, price, createdAt)

**데이터 저장 방식**
- 저장 경로: `data/items.json` (Pretty-print 배열 형식)
- 변경 발생 즉시 `persist()` 호출하여 파일에 반영
- 3계층 구조: 표현 → 비즈니스 → 영속성

**본 프로젝트 적용 시**
- `JsonStorage` 패턴을 그대로 사용하여 `Sample`, `Order`, `ProductionJob`을 각각 JSON 파일로 저장
- 저장 경로 예시: `data/samples.json`, `data/orders.json`, `data/production_jobs.json`

---

### 3. 데이터 모니터링 Tool — [DataMonitor-Andrew](https://github.com/ydh1305/DataMonitor-Andrew)

콘솔에서 JSON 데이터 상태를 실시간 조회하는 관리자 도구.

**핵심 클래스**
- `JsonParser` / `JsonValue` — 재귀 하강 방식 자체 JSON 파서
- `Monitor` — `data/` 디렉토리 감시 및 주기적 갱신
- `ConsoleUI` — ANSI 색상 테이블 렌더링

**사용 방법**
```
DataMonitor.exe [data 경로]   ← 선택적 경로 인자
Q: 종료 / R: 즉시 갱신 / +/-: 갱신 주기 조정
```

**상태 색상 규칙** (본 프로젝트 모니터링에도 적용)
- 초록: 정상 (여유)
- 노랑: 경고 (부족)
- 빨강: 오류 (고갈)

---

### 4. Dummy 데이터 생성 Tool — [DummyDataGenerator-Andrew](https://github.com/ydh1305/DummyDataGenerator-Andrew)

테스트용 더미 데이터를 생성하여 JSON DB에 누적 저장하는 도구.

**핵심 클래스**
- `DummyDataGenerator` — User / Product / Order 3종 데이터 생성
- `JsonDB` — JSON 파일 I/O (`data/db.json`)

**동작 방식**
- 콘솔 메뉴에서 생성할 데이터 종류와 수량 선택
- 기존 `data/db.json`에 이어서 누적 저장 (덮어쓰기 아님)
- 외부 라이브러리 없음, 실행 파일 위치 기준 상대 경로로 `data/` 접근

**본 프로젝트 적용 시**
- `DummyDataGenerator`를 참조하여 `Sample`, `Order` 더미 데이터 생성 기능 구현
- 생성된 더미 데이터는 `data/samples.json`, `data/orders.json`에 적재

---

## 개발 원칙

### 1. 구현 전 사고 (Think Before Coding)
- 가정은 명시적으로 밝히고, 불명확한 경우 구현 전에 질문한다.
- 여러 해석이 가능한 경우 선택지를 제시하고 임의로 결정하지 않는다.
- 더 단순한 접근이 존재하면 먼저 제안하고, 필요하면 반론을 제기한다.

### 2. 단순성 우선 (Simplicity First)
- 요청된 기능만 구현한다. 추측성 기능·추상화·유연성 추가 금지.
- 불가능한 시나리오에 대한 에러 핸들링 추가 금지.
- 명확한 네이밍, 단일 책임 원칙, 주석 최소화. (`docs/code-convention.md` 준수)
- "이 코드가 200줄인데 50줄로 가능한가?" — 가능하면 다시 작성한다.

### 3. 최소 변경 (Surgical Changes)
- 요청된 변경에 직접 관련된 코드만 수정한다.
- 인접한 코드·주석·포맷을 임의로 "개선"하지 않는다.
- 기존 스타일이 다르더라도 프로젝트 스타일을 따른다.
- 내 변경으로 생긴 미사용 import·변수·함수는 제거하되, 기존 dead code는 언급만 한다.

### 4. 목표 기반 실행 (Goal-Driven Execution)
- 작업을 검증 가능한 성공 기준으로 변환한 후 구현한다.
- 다단계 작업은 아래 형식으로 계획을 먼저 제시한다.
  ```
  1. [작업 내용] → 검증: [확인 방법]
  2. [작업 내용] → 검증: [확인 방법]
  ```
- 생산량 계산·상태 전이·재고 판단 로직은 단위 테스트 필수.

### 5. TDD (Test-Driven Development) — 필수
- **모든 기능 구현 및 버그 수정 시 `/test-driven-development` 스킬을 호출하여 진행한다.**
- 구현 순서: **RED** (실패 테스트 작성) → **GREEN** (최소 구현) → **REFACTOR** (정리)
- 프로덕션 코드 작성 전 반드시 실패하는 테스트가 먼저 존재해야 한다.
- 테스트 없이 작성된 프로덕션 코드는 삭제 후 TDD 사이클로 재작성한다.
- 예외 허용 대상 (사용자 확인 필요): Throwaway 프로토타입, 설정 파일, 생성 코드
- **테스트 프레임워크**: Google Mock (gmock) 1.11.0 사용 (NuGet 설치 완료)
  - 테스트 파일은 `SampleOrderSystemTests` 프로젝트에 작성
  - `#include <gmock/gmock.h>` 로 포함
  - `TEST()`, `TEST_F()`, `EXPECT_EQ()`, `ASSERT_EQ()` 등 gtest/gmock 매크로 사용

**REFACTOR 단계는 절대 생략할 수 없다.**
- GREEN 통과 후 REFACTOR 없이 다음 작업으로 넘어가는 것을 금지한다.
- REFACTOR 완료 조건: 중복 제거, 네이밍 개선, 헬퍼 추출 여부를 검토하고 결과를 명시적으로 보고한다.
- 정리할 것이 없더라도 "REFACTOR 검토 완료 — 변경 없음" 을 반드시 출력한다.
- REFACTOR 후 모든 테스트가 GREEN인지 재확인한다.

### 6. 데이터 영속성
- 애플리케이션 재실행 후에도 데이터 유지 (JSON 파일 방식, `DataPersistence-Andrew` 패턴).

### 7. FIFO 스케줄링
- 생산라인 대기 큐는 선입선출 방식으로 처리.

### 8. Commit 이력
- 기능 단위로 커밋. `docs/commit-convention.md` 규칙 준수.
- **커밋은 사용자가 직접 작성한다. Claude는 커밋을 생성하지 않는다.**

## 메인 메뉴 구성

```
[1] 시료 관리        [2] 시료 주문
[3] 주문 승인/거절   [4] 모니터링
[5] 생산라인 조회    [6] 출고 처리
[0] 종료
```

## 주의사항

- REJECTED 주문은 모니터링에서 제외
- 생산라인은 단일 라인, 한 번에 하나의 작업만 처리
- 시료는 시스템에 등록된 것만 주문 가능
- 주문번호 형식: `ORD-YYYYMMDD-NNNN`
