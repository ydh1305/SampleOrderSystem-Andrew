# Commit Convention

[Conventional Commits 1.0.0](https://www.conventionalcommits.org/ko/v1.0.0/) 표준을 따른다.

---

## 기본 형식

```
<type>(<scope>): <subject>

[body]

[footer]
```

- **subject**: 50자 이내, 명령형 현재 시제, 마침표 없음
- **body**: 선택 사항. 변경 이유·맥락 설명. 72자 줄바꿈
- **footer**: 선택 사항. Breaking Change, 이슈 참조

---

## Type 목록

| Type | 사용 시점 |
|------|-----------|
| `feature` | 새로운 기능 추가 |
| `fix` | 버그 수정 |
| `refactor` | 동작 변경 없는 코드 구조 개선 |
| `test` | 테스트 추가·수정 (프로덕션 코드 변경 없음) |
| `docs` | 문서 추가·수정 (코드 변경 없음) |
| `style` | 포맷팅, 세미콜론 누락 등 의미 없는 변경 |
| `chore` | 빌드 스크립트, 설정 파일 등 기타 변경 |
| `perf` | 성능 개선 |

---

## Scope 목록 (프로젝트 한정)

| Scope | 대상 |
|-------|------|
| `sample` | 시료 관리 기능 |
| `order` | 주문 접수·승인·거절 기능 |
| `production` | 생산라인 기능 |
| `release` | 출고 처리 기능 |
| `monitoring` | 모니터링 기능 |
| `storage` | 데이터 영속성 (JsonStorage, Repository) |
| `dummy` | 더미 데이터 생성 도구 |
| `ui` | 콘솔 View 출력 |
| `model` | 데이터 모델 |
| `docs` | 문서 파일 |

---

## 작성 규칙

1. **subject는 한국어 또는 영어 모두 허용**, 프로젝트 내 일관성 유지
2. **type은 소문자**로 작성
3. **Breaking Change**가 있으면 footer에 `BREAKING CHANGE: <설명>` 명시
4. 하나의 커밋은 **하나의 논리적 변경**만 포함
5. WIP(작업 중) 커밋은 `chore: WIP - <내용>` 형식 사용

---

## 예시

```
[feature] order: 주문 접수 시 RESERVED 상태로 주문번호 자동 생성

주문번호는 ORD-YYYYMMDD-NNNN 형식으로 생성하며,
재실행 후에도 순번이 유지되도록 영속성 계층에서 관리한다.
```

```
[fix] production: 실 생산량 계산 시 ceil 미적용 버그 수정
```

```
[feature] storage: JsonStorage에 orders.json CRUD 구현
```

```
[refactor] order: OrderController에서 재고 확인 로직을 OrderService로 분리
```

```
[docs] docs: commit-convention.md 및 code-convention.md 추가
```

```
[test] production: 생산량 계산 공식 단위 테스트 추가
```

```
[chore] .gitignore에 Visual Studio 빌드 산출물 추가
```
