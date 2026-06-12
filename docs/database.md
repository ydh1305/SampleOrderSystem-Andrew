# Database 명세 — JSON 파일 기반 데이터 저장

## 개요

외부 DB 없이 JSON 파일로 데이터를 영속한다. 변경 발생 즉시 파일에 저장한다 (`persist()` 패턴).

| 파일 | 엔티티 | 설명 |
|------|--------|------|
| `data/samples.json` | Sample | 등록된 시료 목록 |
| `data/orders.json` | Order | 전체 주문 목록 |
| `data/production_jobs.json` | ProductionJob | 생산 작업 목록 |

> `data/` 디렉터리는 런타임 생성이며 `.gitignore`에서 제외된다.

---

## 1. data/samples.json

### 스키마

| 필드 | 타입 | 필수 | 설명 |
|------|------|------|------|
| `sampleId` | string | ✓ | 고유 식별자. 형식: `S-{3자리}` |
| `name` | string | ✓ | 시료명 |
| `avgProductionTime` | double | ✓ | 평균 생산시간 (min/ea). 0 초과 |
| `yieldRate` | double | ✓ | 수율 (0.0 < x ≤ 1.0) |
| `stock` | int | ✓ | 현재 재고 수량 (ea). 0 이상 |

### 예시 데이터
```json
[
  {
    "sampleId": "S-001",
    "name": "실리콘 웨이퍼-8인치",
    "avgProductionTime": 0.8,
    "yieldRate": 0.92,
    "stock": 480
  },
  {
    "sampleId": "S-002",
    "name": "GaN 에피택셀-4인치",
    "avgProductionTime": 0.3,
    "yieldRate": 0.88,
    "stock": 220
  },
  {
    "sampleId": "S-003",
    "name": "SiC 파워기판-6인치",
    "avgProductionTime": 0.8,
    "yieldRate": 0.92,
    "stock": 30
  },
  {
    "sampleId": "S-004",
    "name": "산화막 웨이퍼-SiO2",
    "avgProductionTime": 0.6,
    "yieldRate": 0.95,
    "stock": 0
  }
]
```

### 변경 시점
| 작업 | 변경 필드 |
|------|----------|
| 시료 등록 | 신규 레코드 추가 |
| 주문 승인 (재고 충분) | `stock` 차감 |
| 생산 완료 | `stock` 증가 (`floor(actualProd × yieldRate)`) |

---

## 2. data/orders.json

### 스키마

| 필드 | 타입 | 필수 | 설명 |
|------|------|------|------|
| `orderId` | string | ✓ | 주문번호. 형식: `ORD-YYYYMMDD-NNNN` |
| `sampleId` | string | ✓ | 대상 시료 ID (→ samples.json FK) |
| `customerName` | string | ✓ | 고객명 |
| `quantity` | int | ✓ | 주문 수량 (ea). 0 초과 |
| `status` | string | ✓ | 주문 상태 (아래 값 목록 참조) |
| `createdAt` | string | ✓ | 주문 생성일시. 형식: `YYYY-MM-DD HH:MM:SS` |
| `updatedAt` | string | ✓ | 최종 상태 변경일시. 형식: `YYYY-MM-DD HH:MM:SS` |

### status 허용 값

| 값 | 의미 |
|----|------|
| `"RESERVED"` | 주문 접수, 승인 대기 |
| `"REJECTED"` | 주문 거절 (최종 상태) |
| `"PRODUCING"` | 승인 완료, 재고 부족으로 생산 중 |
| `"CONFIRMED"` | 출고 대기 |
| `"RELEASED"` | 출고 완료 (최종 상태) |

### 예시 데이터
```json
[
  {
    "orderId": "ORD-20260612-0001",
    "sampleId": "S-001",
    "customerName": "SK하이닉스",
    "quantity": 150,
    "status": "CONFIRMED",
    "createdAt": "2026-06-12 09:00:00",
    "updatedAt": "2026-06-12 09:15:00"
  },
  {
    "orderId": "ORD-20260612-0002",
    "sampleId": "S-003",
    "customerName": "삼성전자 파운드리",
    "quantity": 200,
    "status": "PRODUCING",
    "createdAt": "2026-06-12 09:05:00",
    "updatedAt": "2026-06-12 09:16:00"
  },
  {
    "orderId": "ORD-20260612-0003",
    "sampleId": "S-002",
    "customerName": "LG이노텍",
    "quantity": 100,
    "status": "RESERVED",
    "createdAt": "2026-06-12 09:10:00",
    "updatedAt": "2026-06-12 09:10:00"
  },
  {
    "orderId": "ORD-20260612-0004",
    "sampleId": "S-004",
    "customerName": "DB하이텍",
    "quantity": 300,
    "status": "REJECTED",
    "createdAt": "2026-06-12 09:12:00",
    "updatedAt": "2026-06-12 09:20:00"
  }
]
```

### 주문번호 순번 규칙
- 형식: `ORD-{YYYYMMDD}-{4자리 zero-padding}`
- 날짜별 독립 순번. 날짜 변경 시 0001 리셋
- `OrderRepository::getNextSequence(date)` 가 기존 레코드에서 최대 순번 추출 후 +1 반환
- 재시작 후에도 파일에서 읽어 이어받으므로 순번 유지

### 변경 시점
| 작업 | 변경 필드 |
|------|----------|
| 주문 접수 | 신규 레코드 추가 (`status=RESERVED`) |
| 주문 승인 (재고 충분) | `status=CONFIRMED`, `updatedAt` |
| 주문 승인 (재고 부족) | `status=PRODUCING`, `updatedAt` |
| 주문 거절 | `status=REJECTED`, `updatedAt` |
| 생산 완료 | `status=CONFIRMED`, `updatedAt` |
| 출고 처리 | `status=RELEASED`, `updatedAt` |

---

## 3. data/production_jobs.json

### 스키마

| 필드 | 타입 | 필수 | 설명 |
|------|------|------|------|
| `jobId` | string | ✓ | 생산 작업 ID. 형식: `JOB-YYYYMMDD-NNNN` |
| `orderId` | string | ✓ | 연결된 주문 ID (→ orders.json FK) |
| `sampleId` | string | ✓ | 생산할 시료 ID (→ samples.json FK) |
| `shortage` | int | ✓ | 부족분 (ea). `quantity - stock` |
| `actualProd` | int | ✓ | 실 생산량 (ea). `ceil(shortage / (yieldRate × 0.9))` |
| `totalTime` | double | ✓ | 총 생산시간 (min). `avgProductionTime × actualProd` |
| `status` | string | ✓ | 작업 상태 (아래 값 목록 참조) |
| `enqueuedAt` | string | ✓ | 큐 등록 시각. 형식: `YYYY-MM-DD HH:MM:SS` |
| `startedAt` | string | - | 생산 시작 시각. WAITING 상태일 때는 `""` |
| `completedAt` | string | - | 생산 완료 시각. DONE 상태일 때만 설정 |

### status 허용 값

| 값 | 의미 |
|----|------|
| `"WAITING"` | 생산 대기 중 (큐에 적재됨) |
| `"RUNNING"` | 현재 생산 중 (단일 라인이므로 동시에 1건만) |
| `"DONE"` | 생산 완료 (최종 상태) |

### 예시 데이터
```json
[
  {
    "jobId": "JOB-20260612-0001",
    "orderId": "ORD-20260612-0002",
    "sampleId": "S-003",
    "shortage": 170,
    "actualProd": 206,
    "totalTime": 164.8,
    "status": "RUNNING",
    "enqueuedAt": "2026-06-12 09:16:00",
    "startedAt": "2026-06-12 09:16:00",
    "completedAt": ""
  },
  {
    "jobId": "JOB-20260612-0002",
    "orderId": "ORD-20260612-0005",
    "sampleId": "S-004",
    "shortage": 300,
    "actualProd": 351,
    "totalTime": 210.6,
    "status": "WAITING",
    "enqueuedAt": "2026-06-12 09:20:00",
    "startedAt": "",
    "completedAt": ""
  }
]
```

### 변경 시점
| 작업 | 변경 필드 |
|------|----------|
| 주문 승인 (재고 부족) | 신규 레코드 추가 (`status=WAITING` 또는 `RUNNING`) |
| 큐 선두 작업 자동 시작 | `status=RUNNING`, `startedAt` |
| 생산 완료 처리 | `status=DONE`, `completedAt` |

---

## 4. 파일 간 관계 (FK)

```
samples.json          orders.json             production_jobs.json
─────────────         ────────────            ────────────────────
sampleId (PK) ←───── sampleId (FK)
                      orderId  (PK) ←───────── orderId  (FK)
                                               sampleId (FK) ──→ samples.json
```

---

## 5. 저장 형식 규칙

- **Pretty-print**: 들여쓰기 2칸, 배열 요소 줄 분리
- **인코딩**: UTF-8
- **최상위 구조**: 항상 JSON 배열 `[...]`
- **빈 상태**: 레코드 없을 때 `[]` (빈 배열)
- **즉시 저장**: CRUD 작업 후 즉시 파일에 반영 (인메모리 캐시 없음)
