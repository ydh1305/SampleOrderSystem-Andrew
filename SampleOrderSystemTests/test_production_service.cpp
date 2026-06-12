#include <gmock/gmock.h>
#include <filesystem>
#include "service/ProductionService.h"
#include "repository/SampleRepository.h"
#include "repository/OrderRepository.h"
#include "repository/ProductionJobRepository.h"

class ProductionServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::string suffix = std::to_string(counter_++);
        sampleRepo_ = std::make_unique<SampleRepository>        ("data/test_ps_sample_" + suffix + ".json");
        orderRepo_  = std::make_unique<OrderRepository>         ("data/test_ps_order_"  + suffix + ".json");
        jobRepo_    = std::make_unique<ProductionJobRepository> ("data/test_ps_job_"    + suffix + ".json");
        svc_ = std::make_unique<ProductionService>(*sampleRepo_, *orderRepo_, *jobRepo_);

        samplePath_ = "data/test_ps_sample_" + suffix + ".json";
        orderPath_  = "data/test_ps_order_"  + suffix + ".json";
        jobPath_    = "data/test_ps_job_"    + suffix + ".json";
    }
    void TearDown() override {
        std::filesystem::remove(samplePath_);
        std::filesystem::remove(orderPath_);
        std::filesystem::remove(jobPath_);
    }

    std::unique_ptr<SampleRepository>        sampleRepo_;
    std::unique_ptr<OrderRepository>         orderRepo_;
    std::unique_ptr<ProductionJobRepository> jobRepo_;
    std::unique_ptr<ProductionService>       svc_;
    std::string samplePath_, orderPath_, jobPath_;
    static int counter_;
};
int ProductionServiceTest::counter_ = 0;

// T14: 생산 완료 -> 재고 증가 + 주문 CONFIRMED + Job DONE
TEST_F(ProductionServiceTest, CompleteJobUpdatesStockAndOrder) {
    sampleRepo_->save({"S-001", "Wafer", 0.8, 0.92, 30});
    orderRepo_->save({"ORD-001", "S-001", "Alice", 200, OrderStatus::PRODUCING, "2026-06-12", ""});
    jobRepo_->save({"JOB-001", "ORD-001", "S-001", 170, 206, 164.8,
                    JobStatus::RUNNING, "2026-06-12 09:00:00", "2026-06-12 09:05:00", ""});

    svc_->completeCurrentJob();

    // goodYield = floor(206 * 0.92) = 189
    EXPECT_EQ(sampleRepo_->findById("S-001")->stock, 30 + 189);
    EXPECT_EQ(orderRepo_->findById("ORD-001")->status, OrderStatus::CONFIRMED);
    EXPECT_EQ(jobRepo_->findById("JOB-001")->status, JobStatus::DONE);
}

// T15: 생산 완료 후 다음 WAITING 작업이 RUNNING으로 자동 시작
TEST_F(ProductionServiceTest, CompleteJobStartsNextWaiting) {
    sampleRepo_->save({"S-001", "Wafer", 0.8, 0.92, 0});
    orderRepo_->save({"ORD-001", "S-001", "Alice", 50, OrderStatus::PRODUCING, "2026-06-12", ""});
    orderRepo_->save({"ORD-002", "S-001", "Bob",   80, OrderStatus::PRODUCING, "2026-06-12", ""});
    jobRepo_->save({"JOB-001", "ORD-001", "S-001", 50, 61, 48.8,
                    JobStatus::RUNNING,  "2026-06-12 09:00:00", "2026-06-12 09:05:00", ""});
    jobRepo_->save({"JOB-002", "ORD-002", "S-001", 80, 98, 78.4,
                    JobStatus::WAITING,  "2026-06-12 09:10:00", "", ""});

    svc_->completeCurrentJob();

    EXPECT_EQ(jobRepo_->findById("JOB-001")->status, JobStatus::DONE);
    EXPECT_EQ(jobRepo_->findById("JOB-002")->status, JobStatus::RUNNING);
}

// T16: RUNNING 작업 없으면 completeCurrentJob -> runtime_error
TEST_F(ProductionServiceTest, CompleteJobThrowsWhenNoRunningJob) {
    EXPECT_THROW(svc_->completeCurrentJob(), std::runtime_error);
}

// T17: startNextJob -> 첫 WAITING 작업을 RUNNING으로 전환
TEST_F(ProductionServiceTest, StartNextJobStartsOldestWaiting) {
    sampleRepo_->save({"S-001", "Wafer", 0.8, 0.92, 0});
    jobRepo_->save({"JOB-001", "ORD-001", "S-001", 50, 61, 48.8,
                    JobStatus::WAITING, "2026-06-12 09:00:00", "", ""});
    jobRepo_->save({"JOB-002", "ORD-002", "S-001", 80, 98, 78.4,
                    JobStatus::WAITING, "2026-06-12 09:10:00", "", ""});

    svc_->startNextJob();

    EXPECT_EQ(jobRepo_->findById("JOB-001")->status, JobStatus::RUNNING);
    EXPECT_EQ(jobRepo_->findById("JOB-002")->status, JobStatus::WAITING);
}
