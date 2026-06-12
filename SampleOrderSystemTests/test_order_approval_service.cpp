#include <gmock/gmock.h>
#include <filesystem>
#include "service/OrderApprovalService.h"
#include "repository/SampleRepository.h"
#include "repository/OrderRepository.h"
#include "repository/ProductionJobRepository.h"

class OrderApprovalServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::string suffix = std::to_string(counter_++);
        sampleRepo_  = std::make_unique<SampleRepository> ("data/test_svc_sample_"  + suffix + ".json");
        orderRepo_   = std::make_unique<OrderRepository>  ("data/test_svc_order_"   + suffix + ".json");
        jobRepo_     = std::make_unique<ProductionJobRepository>("data/test_svc_job_" + suffix + ".json");
        svc_ = std::make_unique<OrderApprovalService>(*sampleRepo_, *orderRepo_, *jobRepo_);

        samplePath_ = "data/test_svc_sample_"  + suffix + ".json";
        orderPath_  = "data/test_svc_order_"   + suffix + ".json";
        jobPath_    = "data/test_svc_job_"     + suffix + ".json";
    }
    void TearDown() override {
        std::filesystem::remove(samplePath_);
        std::filesystem::remove(orderPath_);
        std::filesystem::remove(jobPath_);
    }

    std::unique_ptr<SampleRepository>          sampleRepo_;
    std::unique_ptr<OrderRepository>           orderRepo_;
    std::unique_ptr<ProductionJobRepository>   jobRepo_;
    std::unique_ptr<OrderApprovalService>      svc_;
    std::string samplePath_, orderPath_, jobPath_;
    static int counter_;
};
int OrderApprovalServiceTest::counter_ = 0;

// T9: 재고 충분 -> CONFIRMED + 재고 차감
TEST_F(OrderApprovalServiceTest, ApproveWithSufficientStock) {
    sampleRepo_->save({"S-001", "Wafer", 0.8, 0.92, 200});
    orderRepo_->save({"ORD-001", "S-001", "Alice", 50, OrderStatus::RESERVED, "2026-06-12", ""});

    svc_->approve("ORD-001");

    EXPECT_EQ(orderRepo_->findById("ORD-001")->status, OrderStatus::CONFIRMED);
    EXPECT_EQ(sampleRepo_->findById("S-001")->stock, 150);
    EXPECT_TRUE(jobRepo_->findAll().empty());
}

// T10: 재고 부족 -> PRODUCING + ProductionJob 생성(WAITING)
TEST_F(OrderApprovalServiceTest, ApproveWithInsufficientStock) {
    sampleRepo_->save({"S-001", "Wafer", 0.8, 0.92, 30});
    orderRepo_->save({"ORD-001", "S-001", "Bob", 200, OrderStatus::RESERVED, "2026-06-12", ""});

    svc_->approve("ORD-001");

    EXPECT_EQ(orderRepo_->findById("ORD-001")->status, OrderStatus::PRODUCING);
    auto jobs = jobRepo_->findAll();
    ASSERT_EQ(jobs.size(), 1u);
    EXPECT_EQ(jobs[0].orderId,   "ORD-001");
    EXPECT_EQ(jobs[0].shortage,  170);
    EXPECT_EQ(jobs[0].actualProd, 206);
    EXPECT_EQ(jobs[0].status,    JobStatus::RUNNING);
}

// T11: 거절 -> REJECTED
TEST_F(OrderApprovalServiceTest, RejectChangesStatusToRejected) {
    sampleRepo_->save({"S-001", "Wafer", 0.8, 0.92, 200});
    orderRepo_->save({"ORD-001", "S-001", "Carol", 50, OrderStatus::RESERVED, "2026-06-12", ""});

    svc_->reject("ORD-001");

    EXPECT_EQ(orderRepo_->findById("ORD-001")->status, OrderStatus::REJECTED);
    EXPECT_EQ(sampleRepo_->findById("S-001")->stock, 200);
}

// T12: 재고 0 (완전 부족) -> PRODUCING
TEST_F(OrderApprovalServiceTest, ApproveWithZeroStock) {
    sampleRepo_->save({"S-001", "Wafer", 0.8, 0.92, 0});
    orderRepo_->save({"ORD-001", "S-001", "Dave", 100, OrderStatus::RESERVED, "2026-06-12", ""});

    svc_->approve("ORD-001");

    EXPECT_EQ(orderRepo_->findById("ORD-001")->status, OrderStatus::PRODUCING);
    auto jobs = jobRepo_->findAll();
    ASSERT_EQ(jobs.size(), 1u);
    EXPECT_EQ(jobs[0].shortage, 100);
}

// T13: 존재하지 않는 주문 승인 -> runtime_error
TEST_F(OrderApprovalServiceTest, ApproveNonExistentOrderThrows) {
    EXPECT_THROW(svc_->approve("ORD-NONE"), std::runtime_error);
}
