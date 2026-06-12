# Phase 0 — 프로젝트 셋업

## 목표
Visual Studio 2022 솔루션 생성 및 빌드 가능한 최소 골격 구축.  
이후 모든 Phase가 이 구조 위에서 진행된다.

---

## 작업 항목

### 0-A. Visual Studio 솔루션 & 프로젝트 생성
- [x] 솔루션 이름: `SampleOrderSystem`
- [x] 프로젝트 유형: `콘솔 앱 (C++17, MSVC v143, Windows x64)`
- [x] 문자 집합: `멀티바이트 문자 집합` (ANSI VT100 호환)
- [x] `SampleOrderSystem` — 메인 앱 프로젝트
- [x] `SampleOrderSystemTests` — 테스트 프로젝트 (별도 .vcxproj)

### 0-B. 디렉터리 구조
```
SampleOrderSystem-Andrew/
├── SampleOrderSystem.sln
├── SampleOrderSystem/
│   ├── SampleOrderSystem.vcxproj
│   ├── main.cpp
│   ├── include/              ← 공용 인터페이스 헤더
│   └── src/
│       ├── model/
│       ├── view/
│       ├── controller/
│       ├── repository/
│       └── storage/
├── SampleOrderSystemTests/
│   ├── SampleOrderSystemTests.vcxproj   ← gmock NuGet 참조
│   └── main_test.cpp
└── data/                     ← 런타임 JSON 저장소 (gitignore)
```

### 0-C. main.cpp 최소 진입점
```cpp
#include <iostream>

int main() {
    std::cout << "[S-Semi] 반도체 시료 생산주문관리 시스템 v0.1\n";
    return 0;
}
```

### 0-D. gmock 테스트 프레임워크 설정
- [x] `SampleOrderSystemTests` 프로젝트에 gmock 1.11.0 NuGet 패키지 설치 완료
- [x] `packages.config`에 `gmock.1.11.0` 등록됨
- [x] `.vcxproj`에 `gmock.targets` import 설정됨

**테스트 진입점** (`SampleOrderSystemTests/main_test.cpp`):
```cpp
#include <gmock/gmock.h>

int main(int argc, char** argv) {
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
```

**테스트 파일 작성 패턴**:
```cpp
// 예: SampleOrderSystemTests/test_production_calculator.cpp
#include <gtest/gtest.h>
#include "model/ProductionCalculator.h"

TEST(ProductionCalculatorTest, BasicCalculation) {
    auto result = ProductionCalculator::calculate(200, 30, 0.92, 0.8);
    EXPECT_EQ(result.shortage, 170);
    EXPECT_EQ(result.actualProd, 206);
}
```

**Fixture 기반 테스트 패턴** (상태 공유 필요 시):
```cpp
class SampleRepositoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        filePath_ = "data/test_sample_" + std::to_string(testCounter_++) + ".json";
        repo_ = std::make_unique<SampleRepository>(filePath_);
    }
    void TearDown() override {
        std::filesystem::remove(filePath_);
    }
    std::string filePath_;
    std::unique_ptr<SampleRepository> repo_;
    static int testCounter_;
};
int SampleRepositoryTest::testCounter_ = 0;
```

### 0-E. .gitignore 업데이트
```
data/
x64/
Debug/
Release/
*.exe
*.pdb
*.ilk
packages/   ← NuGet 복원 대상, 커밋 불필요
```

---

## 산출물

| 파일 | 설명 |
|------|------|
| `SampleOrderSystem.sln` | VS 솔루션 (완료) |
| `SampleOrderSystem/main.cpp` | 진입점 (완료) |
| `SampleOrderSystemTests/main_test.cpp` | gmock RUN_ALL_TESTS() 진입점 |

---

## 검증 기준
- [x] `main.cpp` 빌드 성공
- [ ] `SampleOrderSystemTests` 프로젝트 빌드 성공 (빈 테스트 0건 PASS)
- [ ] `RUN_ALL_TESTS()` 실행 시 "0 tests run" 출력 (오류 없음)
- [ ] `data/` 디렉터리 `.gitignore`에 포함됨

---

## 비고
- 자체 테스트 매크로(`ASSERT_EQ`, `TestRunner.h`) **사용 금지** — gmock/gtest만 사용
- `packages/` 디렉터리는 NuGet 복원으로 재생성되므로 gitignore 처리
- `SampleOrderSystemTests`의 AdditionalIncludeDirectories에 메인 프로젝트의 `src/`, `include/` 경로 추가 필요
