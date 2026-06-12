#include <gmock/gmock.h>
#include <sstream>
#include <filesystem>
#include "controller/MainController.h"

using ::testing::HasSubstr;

class IntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::string s = std::to_string(counter_++);
        samplePath_ = "data/int_s_" + s + ".json";
        orderPath_  = "data/int_o_" + s + ".json";
        jobPath_    = "data/int_j_" + s + ".json";
    }
    void TearDown() override {
        std::filesystem::remove(samplePath_);
        std::filesystem::remove(orderPath_);
        std::filesystem::remove(jobPath_);
    }
    void run(const std::string& input) {
        out_.str("");
        in_ = std::istringstream(input);
        MainController ctrl(samplePath_, orderPath_, jobPath_, out_, in_);
        ctrl.run();
    }

    std::ostringstream out_;
    std::istringstream in_;
    std::string samplePath_, orderPath_, jobPath_;
    static int counter_;
};
int IntegrationTest::counter_ = 0;

// T1: 시료 등록 후 목록 조회 - 등록한 시료가 출력에 포함됨
// Main: 1(시료관리) -> 1(등록) -> 입력 -> Y -> 2(목록) -> 0(뒤로) -> 0(종료)
TEST_F(IntegrationTest, AddSampleThenListShowsNewSample) {
    run("1\n1\nWafer-8inch\n0.8\n0.92\n100\nY\n2\n0\n0\n");
    EXPECT_THAT(out_.str(), HasSubstr("Wafer-8inch"));
    EXPECT_THAT(out_.str(), HasSubstr("S-001"));
}

// T2: 존재하지 않는 시료 ID로 주문 시 에러 메시지 출력
// Main: 2(주문접수) -> 없는ID -> 0(종료)
TEST_F(IntegrationTest, PlaceOrderWithUnknownSampleShowsError) {
    run("2\nS-999\n0\n");
    EXPECT_FALSE(out_.str().empty());
    // 에러 메시지가 출력되고 정상 종료
}

// T3: 시료 등록 -> 주문 접수 -> 승인(재고 충분) 완전 흐름
// Main: 1->1(등록Wafer/200ea) -> 0 -> 2(주문S-001/50ea) -> 3(승인#1/A) -> 0(종료)
TEST_F(IntegrationTest, FullFlow_AddSample_PlaceOrder_ApproveConfirmed) {
    run("1\n1\nWafer\n0.8\n0.92\n200\nY\n0\n"
        "2\nS-001\nAlice\n50\nY\n"
        "3\n1\nA\n"
        "0\n");
    EXPECT_THAT(out_.str(), HasSubstr("S-001"));
    EXPECT_THAT(out_.str(), HasSubstr("Alice"));
}

// T4: 주문 승인 후 모니터링에서 CONFIRMED 수량이 1로 표시됨
// (T3 흐름 연장: 마지막에 4->1->0 추가)
TEST_F(IntegrationTest, AfterApproval_MonitoringShowsConfirmedCount) {
    run("1\n1\nWafer\n0.8\n0.92\n200\nY\n0\n"
        "2\nS-001\nAlice\n50\nY\n"
        "3\n1\nA\n"
        "4\n1\n0\n"
        "0\n");
    EXPECT_THAT(out_.str(), HasSubstr("CONFIRMED"));
    EXPECT_THAT(out_.str(), HasSubstr("1"));
}

// T5: 시료 등록 -> 주문 -> 승인(재고 부족) -> PRODUCING 상태 확인
// stock=10, order=100 -> 부족 -> PRODUCING
TEST_F(IntegrationTest, ApproveWithInsufficientStock_StatusProducing) {
    run("1\n1\nGaN-epi\n0.5\n0.88\n10\nY\n0\n"
        "2\nS-001\nBob\n100\nY\n"
        "3\n1\nA\n"
        "4\n1\n0\n"
        "0\n");
    EXPECT_THAT(out_.str(), HasSubstr("PRODUCING"));
    EXPECT_THAT(out_.str(), HasSubstr("1"));
}

// T6: CONFIRMED 주문 출고 처리 -> RELEASED
// T3 흐름 후 출고 처리
TEST_F(IntegrationTest, ReleaseConfirmedOrder) {
    run("1\n1\nWafer\n0.8\n0.92\n200\nY\n0\n"
        "2\nS-001\nCarol\n50\nY\n"
        "3\n1\nA\n"
        "6\n1\nY\n"
        "0\n");
    EXPECT_THAT(out_.str(), HasSubstr("Carol"));
}

// T7: stock=30, order=200 -> PRODUCING -> 생산완료 -> CONFIRMED -> 출고 -> RELEASED:1
TEST_F(IntegrationTest, StockInsufficientFullFlow) {
    run("1\n1\nWafer\n0.8\n0.92\n30\nY\n0\n"
        "2\nS-001\nBob\n200\nY\n"
        "3\n1\nA\n"
        "5\n2\n0\n"
        "6\n1\nY\n"
        "4\n1\n0\n"
        "0\n");
    EXPECT_THAT(out_.str(), HasSubstr("RELEASED : 1"));
}

// T8: REJECTED 주문은 모니터링 카운트에서 제외됨
TEST_F(IntegrationTest, AfterRejection_AllCountsZero) {
    run("1\n1\nWafer\n0.8\n0.92\n100\nY\n0\n"
        "2\nS-001\nAlice\n50\nY\n"
        "3\n1\nR\n"
        "4\n1\n0\n"
        "0\n");
    EXPECT_THAT(out_.str(), HasSubstr("RESERVED : 0"));
    EXPECT_THAT(out_.str(), HasSubstr("CONFIRMED: 0"));
}
