#include <gmock/gmock.h>
#include <sstream>
#include <map>
#include "view/SampleView.h"
#include "view/OrderView.h"
#include "view/MonitoringView.h"
#include "view/ReleaseView.h"
#include "view/ProductionView.h"
#include "view/MainView.h"

using ::testing::HasSubstr;

// T1: 서브메뉴 출력 확인
TEST(SampleViewTest, ShowSubMenuContainsOptions) {
    std::ostringstream out; std::istringstream in;
    SampleView v(out, in);
    v.showSubMenu();
    EXPECT_THAT(out.str(), HasSubstr("[1]"));
    EXPECT_THAT(out.str(), HasSubstr("[0]"));
}

// T2: 시료 목록 출력 - sampleId 포함
TEST(SampleViewTest, ShowSampleListContainsId) {
    std::ostringstream out; std::istringstream in;
    SampleView v(out, in);
    Sample s{"S-001", "Wafer", 0.8, 0.92, 100};
    v.showSampleList({s});
    EXPECT_THAT(out.str(), HasSubstr("S-001"));
    EXPECT_THAT(out.str(), HasSubstr("Wafer"));
}

// T3: 시료 없을 때 메시지 출력
TEST(SampleViewTest, ShowNoSamplesOutputsMessage) {
    std::ostringstream out; std::istringstream in;
    SampleView v(out, in);
    v.showNoSamples();
    EXPECT_FALSE(out.str().empty());
}

// T4: 메뉴 선택 입력 읽기
TEST(SampleViewTest, ReadMenuChoiceReturnsInteger) {
    std::ostringstream out; std::istringstream in("2\n");
    SampleView v(out, in);
    EXPECT_EQ(v.readMenuChoice(), 2);
}

// T5: 시료명 입력 읽기
TEST(SampleViewTest, ReadNameReturnsInputString) {
    std::ostringstream out; std::istringstream in("Wafer-8inch\n");
    SampleView v(out, in);
    EXPECT_EQ(v.readName(), "Wafer-8inch");
}

// T6: 등록 성공 메시지 출력
TEST(SampleViewTest, ShowSaveSuccessContainsId) {
    std::ostringstream out; std::istringstream in;
    SampleView v(out, in);
    v.showSaveSuccess("S-001");
    EXPECT_THAT(out.str(), HasSubstr("S-001"));
}

// T7: RESERVED 주문 목록 출력
TEST(OrderViewTest, ShowReservedOrdersContainsOrderId) {
    std::ostringstream out; std::istringstream in;
    OrderView v(out, in);
    Order o{"ORD-20260612-0001", "S-001", "Alice", 50, OrderStatus::RESERVED, "2026-06-12", ""};
    v.showReservedOrders({o});
    EXPECT_THAT(out.str(), HasSubstr("ORD-20260612-0001"));
    EXPECT_THAT(out.str(), HasSubstr("Alice"));
}

// T8: 주문 없을 때 메시지
TEST(OrderViewTest, ShowNoReservedOrdersOutputsMessage) {
    std::ostringstream out; std::istringstream in;
    OrderView v(out, in);
    v.showNoReservedOrders();
    EXPECT_FALSE(out.str().empty());
}

// T9: 수량 입력
TEST(OrderViewTest, ReadQuantityReturnsInteger) {
    std::ostringstream out; std::istringstream in("100\n");
    OrderView v(out, in);
    EXPECT_EQ(v.readQuantity(), 100);
}

// T10: 주문 생성 완료 출력
TEST(OrderViewTest, ShowOrderCreatedContainsOrderId) {
    std::ostringstream out; std::istringstream in;
    OrderView v(out, in);
    Order o{"ORD-20260612-0001", "S-001", "Bob", 50, OrderStatus::RESERVED, "2026-06-12", ""};
    v.showOrderCreated(o);
    EXPECT_THAT(out.str(), HasSubstr("ORD-20260612-0001"));
}

// T11: Y/N 입력
TEST(OrderViewTest, ReadYesNoReturnsChar) {
    std::ostringstream out; std::istringstream in("Y\n");
    OrderView v(out, in);
    EXPECT_EQ(v.readYesNo(), 'Y');
}

// MonitoringView
TEST(MonitoringViewTest, ShowSubMenuContainsOptions) {
    std::ostringstream out; std::istringstream in;
    MonitoringView v(out, in);
    v.showSubMenu();
    EXPECT_THAT(out.str(), HasSubstr("[1]"));
    EXPECT_THAT(out.str(), HasSubstr("[2]"));
}

TEST(MonitoringViewTest, ShowOrderSummaryContainsReservedCount) {
    std::ostringstream out; std::istringstream in;
    MonitoringView v(out, in);
    std::map<OrderStatus, int> counts = {
        {OrderStatus::RESERVED, 3}, {OrderStatus::CONFIRMED, 5},
        {OrderStatus::PRODUCING, 2}, {OrderStatus::RELEASED, 10}
    };
    v.showOrderSummary(counts);
    EXPECT_THAT(out.str(), HasSubstr("3"));
    EXPECT_THAT(out.str(), HasSubstr("RESERVED"));
}

TEST(MonitoringViewTest, ShowStockStatusContainsSampleName) {
    std::ostringstream out; std::istringstream in;
    MonitoringView v(out, in);
    Sample s{"S-001", "Wafer", 0.8, 0.92, 100};
    std::vector<SampleStockInfo> info = {{s, 50, "여유"}};
    v.showStockStatus(info);
    EXPECT_THAT(out.str(), HasSubstr("Wafer"));
    EXPECT_THAT(out.str(), HasSubstr("100"));
}

// ReleaseView
TEST(ReleaseViewTest, ShowConfirmedOrdersContainsOrderId) {
    std::ostringstream out; std::istringstream in;
    ReleaseView v(out, in);
    Order o{"ORD-20260612-0001", "S-001", "Carol", 50, OrderStatus::CONFIRMED, "2026-06-12", ""};
    v.showConfirmedOrders({o});
    EXPECT_THAT(out.str(), HasSubstr("ORD-20260612-0001"));
}

TEST(ReleaseViewTest, ShowNoConfirmedOrdersOutputsMessage) {
    std::ostringstream out; std::istringstream in;
    ReleaseView v(out, in);
    v.showNoConfirmedOrders();
    EXPECT_FALSE(out.str().empty());
}

TEST(ReleaseViewTest, ShowReleaseSuccessContainsOrderId) {
    std::ostringstream out; std::istringstream in;
    ReleaseView v(out, in);
    Order o{"ORD-20260612-0001", "S-001", "Dave", 50, OrderStatus::RELEASED, "2026-06-12", "2026-06-12"};
    v.showReleaseSuccess(o);
    EXPECT_THAT(out.str(), HasSubstr("ORD-20260612-0001"));
}

// ProductionView
TEST(ProductionViewTest, ShowNoRunningJobOutputsMessage) {
    std::ostringstream out; std::istringstream in;
    ProductionView v(out, in);
    v.showNoRunningJob();
    EXPECT_FALSE(out.str().empty());
}

TEST(ProductionViewTest, ShowRunningJobContainsJobId) {
    std::ostringstream out; std::istringstream in;
    ProductionView v(out, in);
    ProductionJob job{"JOB-001", "ORD-001", "S-001", 170, 206, 164.8,
                      JobStatus::RUNNING, "2026-06-12 09:00", "2026-06-12 09:05", ""};
    Sample s{"S-001", "Wafer", 0.8, 0.92, 30};
    v.showRunningJob(job, s);
    EXPECT_THAT(out.str(), HasSubstr("JOB-001"));
    EXPECT_THAT(out.str(), HasSubstr("206"));
}

TEST(ProductionViewTest, ShowWaitingQueueContainsJobId) {
    std::ostringstream out; std::istringstream in;
    ProductionView v(out, in);
    ProductionJob job{"JOB-002", "ORD-002", "S-001", 80, 97, 77.6,
                      JobStatus::WAITING, "2026-06-12 09:10", "", ""};
    v.showWaitingQueue({job});
    EXPECT_THAT(out.str(), HasSubstr("JOB-002"));
}

TEST(ProductionViewTest, ShowCompleteMenuContainsOption) {
    std::ostringstream out; std::istringstream in;
    ProductionView v(out, in);
    v.showCompleteMenu(true);
    EXPECT_THAT(out.str(), HasSubstr("[C]"));
}

// MainView
TEST(MainViewTest, ShowMainMenuContainsAllOptions) {
    std::ostringstream out; std::istringstream in;
    MainView v(out, in);
    v.showMainMenu();
    EXPECT_THAT(out.str(), HasSubstr("[1]"));
    EXPECT_THAT(out.str(), HasSubstr("[6]"));
    EXPECT_THAT(out.str(), HasSubstr("[0]"));
}

TEST(MainViewTest, ShowSummaryContainsCounts) {
    std::ostringstream out; std::istringstream in;
    MainView v(out, in);
    v.showSummary(5, 1200, 10, 2);
    EXPECT_THAT(out.str(), HasSubstr("5"));
    EXPECT_THAT(out.str(), HasSubstr("1200"));
}

TEST(MainViewTest, ReadMenuChoiceReturnsInteger) {
    std::ostringstream out; std::istringstream in("3\n");
    MainView v(out, in);
    EXPECT_EQ(v.readMenuChoice(), 3);
}
