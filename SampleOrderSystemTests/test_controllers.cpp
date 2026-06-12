#include <gmock/gmock.h>
#include <sstream>
#include <filesystem>
#include "mocks/MockRepositories.h"
#include "controller/SampleController.h"
#include "controller/OrderController.h"
#include "controller/MonitoringController.h"
#include "controller/ReleaseController.h"
#include "controller/ProductionController.h"
#include "controller/MainController.h"

using ::testing::Return;
using ::testing::_;
using ::testing::AtLeast;

// ==================== SampleControllerTest ====================

class SampleControllerTest : public ::testing::Test {
protected:
    MockSampleRepository mockRepo_;
    std::ostringstream out_;
    std::istringstream in_;
};

// T1: listSamples calls findAll
TEST_F(SampleControllerTest, ListSamplesCallsFindAll) {
    std::vector<Sample> samples = {{"S-001", "Wafer", 0.8, 0.92, 100}};
    EXPECT_CALL(mockRepo_, findAll()).WillOnce(Return(samples));
    SampleController ctrl(mockRepo_, out_, in_);
    ctrl.listSamples();
    EXPECT_THAT(out_.str(), ::testing::HasSubstr("S-001"));
}

// T2: addSample calls generateNextId then save
TEST_F(SampleControllerTest, AddSampleCallsSave) {
    EXPECT_CALL(mockRepo_, findAll()).WillRepeatedly(Return(std::vector<Sample>{}));
    EXPECT_CALL(mockRepo_, generateNextId()).WillOnce(Return("S-001"));
    EXPECT_CALL(mockRepo_, save(_)).Times(1);
    in_.str("Wafer\n0.8\n0.92\n100\nY\n");
    SampleController ctrl(mockRepo_, out_, in_);
    ctrl.addSample();
}

// T3: searchSamples filters by name (case-insensitive substring)
TEST_F(SampleControllerTest, SearchSamplesFiltersByName) {
    std::vector<Sample> all = {
        {"S-001", "Wafer-8inch", 0.8, 0.92, 100},
        {"S-002", "GaN-epi", 0.5, 0.88, 50}
    };
    EXPECT_CALL(mockRepo_, findAll()).WillOnce(Return(all));
    in_.str("wafer\n");
    SampleController ctrl(mockRepo_, out_, in_);
    ctrl.searchSamples();
    EXPECT_THAT(out_.str(), ::testing::HasSubstr("Wafer-8inch"));
}

// ==================== OrderControllerTest ====================

class OrderControllerTest : public ::testing::Test {
protected:
    MockSampleRepository mockSampleRepo_;
    MockOrderRepository mockOrderRepo_;
    MockProductionJobRepository mockJobRepo_;
    std::ostringstream out_;
    std::istringstream in_;
};

// T4: placeOrder saves with RESERVED status
TEST_F(OrderControllerTest, PlaceOrderSavesWithReservedStatus) {
    Sample s{"S-001", "Wafer", 0.8, 0.92, 100};
    EXPECT_CALL(mockSampleRepo_, findById("S-001")).WillOnce(Return(s));
    EXPECT_CALL(mockOrderRepo_, getNextSequence(_)).WillOnce(Return(1));
    EXPECT_CALL(mockOrderRepo_, save(_)).WillOnce([](const Order& o) {
        EXPECT_EQ(o.status, OrderStatus::RESERVED);
        EXPECT_EQ(o.sampleId, "S-001");
    });
    in_.str("S-001\nAlice\n50\nY\n");
    OrderController ctrl(mockSampleRepo_, mockOrderRepo_, mockJobRepo_, out_, in_);
    ctrl.placeOrder();
}

// T5: approveOrder calls service.approve
TEST_F(OrderControllerTest, ApproveOrderCallsApprove) {
    std::vector<Order> reserved = {
        {"ORD-001", "S-001", "Alice", 50, OrderStatus::RESERVED, "2026-06-12", ""}
    };
    EXPECT_CALL(mockOrderRepo_, findByStatus(OrderStatus::RESERVED)).WillOnce(Return(reserved));
    Sample s{"S-001", "Wafer", 0.8, 0.92, 200};
    EXPECT_CALL(mockSampleRepo_, findById("S-001")).WillRepeatedly(Return(s));
    EXPECT_CALL(mockSampleRepo_, update(_)).Times(1);
    EXPECT_CALL(mockOrderRepo_, findById("ORD-001")).WillOnce(Return(reserved[0]));
    EXPECT_CALL(mockOrderRepo_, update(_)).Times(1);
    in_.str("1\nA\n");
    OrderController ctrl(mockSampleRepo_, mockOrderRepo_, mockJobRepo_, out_, in_);
    ctrl.processApproval();
}

// T6: rejectOrder changes status to REJECTED
TEST_F(OrderControllerTest, RejectOrderChangesStatus) {
    std::vector<Order> reserved = {
        {"ORD-001", "S-001", "Alice", 50, OrderStatus::RESERVED, "2026-06-12", ""}
    };
    EXPECT_CALL(mockOrderRepo_, findByStatus(OrderStatus::RESERVED)).WillOnce(Return(reserved));
    EXPECT_CALL(mockOrderRepo_, findById("ORD-001")).WillOnce(Return(reserved[0]));
    EXPECT_CALL(mockOrderRepo_, update(_)).WillOnce([](const Order& o) {
        EXPECT_EQ(o.status, OrderStatus::REJECTED);
    });
    in_.str("1\nR\n");
    OrderController ctrl(mockSampleRepo_, mockOrderRepo_, mockJobRepo_, out_, in_);
    ctrl.processApproval();
}

// ==================== MonitoringControllerTest ====================

class MonitoringControllerTest : public ::testing::Test {
protected:
    MockSampleRepository mockSampleRepo_;
    MockOrderRepository mockOrderRepo_;
    std::ostringstream out_;
    std::istringstream in_;
};

// T7: showOrderCounts calls findAll on orderRepo
TEST_F(MonitoringControllerTest, ShowOrderCountsCallsFindAll) {
    std::vector<Order> orders = {
        {"ORD-001", "S-001", "A", 10, OrderStatus::RESERVED, "2026-06-12", ""},
        {"ORD-002", "S-001", "B", 20, OrderStatus::CONFIRMED, "2026-06-12", ""}
    };
    EXPECT_CALL(mockOrderRepo_, findAll()).WillOnce(Return(orders));
    MonitoringController ctrl(mockSampleRepo_, mockOrderRepo_, out_, in_);
    ctrl.showOrderCounts();
    EXPECT_THAT(out_.str(), ::testing::HasSubstr("RESERVED"));
    EXPECT_THAT(out_.str(), ::testing::HasSubstr("CONFIRMED"));
}

// T8: showStockStatus calls findAll on both repos
TEST_F(MonitoringControllerTest, ShowStockStatusCallsBothRepos) {
    EXPECT_CALL(mockSampleRepo_, findAll()).WillOnce(Return(std::vector<Sample>{}));
    EXPECT_CALL(mockOrderRepo_, findAll()).WillOnce(Return(std::vector<Order>{}));
    MonitoringController ctrl(mockSampleRepo_, mockOrderRepo_, out_, in_);
    ctrl.showStockStatus();
}

// 재고 고갈(stock=0) -> 빨간색(\033[31m) 코드 출력
TEST_F(MonitoringControllerTest, StockDepletedShowsRedColor) {
    std::vector<Sample> samples = {{"S-001", "Wafer", 0.8, 0.92, 0}};
    EXPECT_CALL(mockSampleRepo_, findAll()).WillOnce(Return(samples));
    EXPECT_CALL(mockOrderRepo_, findAll()).WillOnce(Return(std::vector<Order>{}));
    MonitoringController ctrl(mockSampleRepo_, mockOrderRepo_, out_, in_);
    ctrl.showStockStatus();
    EXPECT_THAT(out_.str(), ::testing::HasSubstr("\033[31m"));
}

// 재고 부족(RESERVED+CONFIRMED > stock) -> 노란색(\033[33m) 코드 출력
TEST_F(MonitoringControllerTest, StockShortShowsYellowColor) {
    std::vector<Sample> samples = {{"S-001", "Wafer", 0.8, 0.92, 10}};
    std::vector<Order> orders = {
        {"ORD-001", "S-001", "Alice", 15, OrderStatus::RESERVED, "2026-06-12", ""}
    };
    EXPECT_CALL(mockSampleRepo_, findAll()).WillOnce(Return(samples));
    EXPECT_CALL(mockOrderRepo_, findAll()).WillOnce(Return(orders));
    MonitoringController ctrl(mockSampleRepo_, mockOrderRepo_, out_, in_);
    ctrl.showStockStatus();
    EXPECT_THAT(out_.str(), ::testing::HasSubstr("\033[33m"));
}

// 재고 여유(stock >= 대기 수량) -> 초록색(\033[32m) 코드 출력
TEST_F(MonitoringControllerTest, StockSufficientShowsGreenColor) {
    std::vector<Sample> samples = {{"S-001", "Wafer", 0.8, 0.92, 100}};
    std::vector<Order> orders = {
        {"ORD-001", "S-001", "Alice", 20, OrderStatus::RESERVED, "2026-06-12", ""}
    };
    EXPECT_CALL(mockSampleRepo_, findAll()).WillOnce(Return(samples));
    EXPECT_CALL(mockOrderRepo_, findAll()).WillOnce(Return(orders));
    MonitoringController ctrl(mockSampleRepo_, mockOrderRepo_, out_, in_);
    ctrl.showStockStatus();
    EXPECT_THAT(out_.str(), ::testing::HasSubstr("\033[32m"));
}

// ==================== ReleaseControllerTest ====================

class ReleaseControllerTest : public ::testing::Test {
protected:
    MockOrderRepository mockOrderRepo_;
    std::ostringstream out_;
    std::istringstream in_;
};

// T9: processRelease updates status to RELEASED
TEST_F(ReleaseControllerTest, ProcessReleaseUpdatesStatusToReleased) {
    Order confirmed{"ORD-001", "S-001", "Alice", 50, OrderStatus::CONFIRMED, "2026-06-12", ""};
    EXPECT_CALL(mockOrderRepo_, findByStatus(OrderStatus::CONFIRMED)).WillOnce(Return(std::vector<Order>{confirmed}));
    EXPECT_CALL(mockOrderRepo_, update(_)).WillOnce([](const Order& o) {
        EXPECT_EQ(o.status, OrderStatus::RELEASED);
    });
    in_.str("1\nY\n");
    ReleaseController ctrl(mockOrderRepo_, out_, in_);
    ctrl.processRelease();
}

// T10: processRelease shows message when no confirmed orders
TEST_F(ReleaseControllerTest, ProcessReleaseShowsMessageWhenEmpty) {
    EXPECT_CALL(mockOrderRepo_, findByStatus(OrderStatus::CONFIRMED)).WillOnce(Return(std::vector<Order>{}));
    ReleaseController ctrl(mockOrderRepo_, out_, in_);
    ctrl.processRelease();
    EXPECT_FALSE(out_.str().empty());
}

// ==================== ProductionControllerTest ====================

class ProductionControllerTest : public ::testing::Test {
protected:
    MockSampleRepository mockSampleRepo_;
    MockOrderRepository mockOrderRepo_;
    MockProductionJobRepository mockJobRepo_;
    std::ostringstream out_;
    std::istringstream in_;
};

// T11: showProductionLine calls findByStatus for RUNNING and WAITING
TEST_F(ProductionControllerTest, ShowProductionLineCallsFindRunning) {
    EXPECT_CALL(mockJobRepo_, findByStatus(JobStatus::RUNNING)).WillOnce(Return(std::vector<ProductionJob>{}));
    EXPECT_CALL(mockJobRepo_, findByStatus(JobStatus::WAITING)).WillOnce(Return(std::vector<ProductionJob>{}));
    ProductionController ctrl(mockSampleRepo_, mockOrderRepo_, mockJobRepo_, out_, in_);
    ctrl.showProductionLine();
}

// T12: completeCurrentJob calls service
TEST_F(ProductionControllerTest, CompleteCurrentJobCallsService) {
    ProductionJob running{"JOB-001", "ORD-001", "S-001", 170, 206, 164.8,
                          JobStatus::RUNNING, "2026-06-12 09:00", "2026-06-12 09:05", ""};
    EXPECT_CALL(mockJobRepo_, findByStatus(JobStatus::RUNNING)).WillRepeatedly(Return(std::vector<ProductionJob>{running}));
    EXPECT_CALL(mockJobRepo_, findRunning()).WillRepeatedly(Return(running));
    Sample s{"S-001", "Wafer", 0.8, 0.92, 30};
    EXPECT_CALL(mockSampleRepo_, findById("S-001")).WillRepeatedly(Return(s));
    EXPECT_CALL(mockSampleRepo_, update(_)).Times(1);
    Order o{"ORD-001", "S-001", "A", 200, OrderStatus::PRODUCING, "2026-06-12", ""};
    EXPECT_CALL(mockOrderRepo_, findById("ORD-001")).WillRepeatedly(Return(o));
    EXPECT_CALL(mockOrderRepo_, update(_)).Times(1);
    EXPECT_CALL(mockJobRepo_, update(_)).Times(AtLeast(1));
    EXPECT_CALL(mockJobRepo_, findWaiting()).WillRepeatedly(Return(std::vector<ProductionJob>{}));
    ProductionController ctrl(mockSampleRepo_, mockOrderRepo_, mockJobRepo_, out_, in_);
    ctrl.completeCurrentJob();
}

// T13: MainController run loops until 0
TEST(MainControllerTest, RunLoopsUntilZeroInput) {
    std::ostringstream out;
    std::istringstream in("0\n");
    MainController ctrl("data/test_mc_samples.json", "data/test_mc_orders.json",
                        "data/test_mc_jobs.json", out, in);
    ctrl.run();
    std::filesystem::remove("data/test_mc_samples.json");
    std::filesystem::remove("data/test_mc_orders.json");
    std::filesystem::remove("data/test_mc_jobs.json");
}
