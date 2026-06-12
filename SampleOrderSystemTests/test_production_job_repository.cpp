#include <gmock/gmock.h>
#include <filesystem>
#include "repository/ProductionJobRepository.h"

class ProductionJobRepositoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        testPath_ = "data/test_job_" + std::to_string(counter_++) + ".json";
        repo_ = std::make_unique<ProductionJobRepository>(testPath_);
    }
    void TearDown() override { std::filesystem::remove(testPath_); }
    std::string testPath_;
    std::unique_ptr<ProductionJobRepository> repo_;
    static int counter_;
};
int ProductionJobRepositoryTest::counter_ = 0;

// T12: findAll returns empty initially
TEST_F(ProductionJobRepositoryTest, FindAllReturnsEmptyInitially) {
    EXPECT_TRUE(repo_->findAll().empty());
}

// T13: save then findById returns saved job
TEST_F(ProductionJobRepositoryTest, SaveAndFindById) {
    ProductionJob job{"JOB-001", "ORD-20260612-0001", "S-001", 30, 40, 5.0,
                      JobStatus::WAITING, "2026-06-12", "", ""};
    repo_->save(job);
    auto found = repo_->findById("JOB-001");
    ASSERT_TRUE(found.has_value());
    EXPECT_EQ(found->orderId,    "ORD-20260612-0001");
    EXPECT_EQ(found->actualProd, 40);
}

// T14: update changes status
TEST_F(ProductionJobRepositoryTest, UpdateChangesStatus) {
    ProductionJob job{"JOB-001", "ORD-20260612-0001", "S-001", 30, 40, 5.0,
                      JobStatus::WAITING, "2026-06-12", "", ""};
    repo_->save(job);
    job.status = JobStatus::RUNNING;
    repo_->update(job);
    EXPECT_EQ(repo_->findById("JOB-001")->status, JobStatus::RUNNING);
}

// T15: findByStatus returns only matching jobs
TEST_F(ProductionJobRepositoryTest, FindByStatusReturnsMatching) {
    repo_->save({"JOB-001", "ORD-001", "S-001", 10, 12, 2.0, JobStatus::WAITING,  "2026-06-12", "", ""});
    repo_->save({"JOB-002", "ORD-002", "S-001", 20, 25, 3.0, JobStatus::RUNNING,  "2026-06-12", "2026-06-12", ""});
    repo_->save({"JOB-003", "ORD-003", "S-002", 15, 18, 2.5, JobStatus::WAITING,  "2026-06-12", "", ""});

    auto waiting = repo_->findByStatus(JobStatus::WAITING);
    ASSERT_EQ(waiting.size(), 2u);
    auto running = repo_->findByStatus(JobStatus::RUNNING);
    ASSERT_EQ(running.size(), 1u);
    EXPECT_EQ(running[0].jobId, "JOB-002");
}

// T16: generateNextId returns JOB-001 when empty, increments correctly
TEST_F(ProductionJobRepositoryTest, GenerateNextIdIncrements) {
    EXPECT_EQ(repo_->generateNextId(), "JOB-001");
    repo_->save({"JOB-001", "ORD-001", "S-001", 10, 12, 2.0, JobStatus::WAITING, "2026-06-12", "", ""});
    repo_->save({"JOB-002", "ORD-002", "S-001", 20, 25, 3.0, JobStatus::WAITING, "2026-06-12", "", ""});
    EXPECT_EQ(repo_->generateNextId(), "JOB-003");
}

// T17: findRunning returns the RUNNING job
TEST_F(ProductionJobRepositoryTest, FindRunningReturnsRunningJob) {
    repo_->save({"JOB-001", "ORD-001", "S-001", 10, 12, 2.0, JobStatus::WAITING,  "2026-06-12 09:00:00", "",                   ""});
    repo_->save({"JOB-002", "ORD-002", "S-001", 20, 25, 3.0, JobStatus::RUNNING,  "2026-06-12 09:05:00", "2026-06-12 09:10:00", ""});
    auto running = repo_->findRunning();
    ASSERT_TRUE(running.has_value());
    EXPECT_EQ(running->jobId, "JOB-002");
}

// T18: findRunning returns nullopt when no RUNNING job
TEST_F(ProductionJobRepositoryTest, FindRunningReturnsNulloptWhenNone) {
    repo_->save({"JOB-001", "ORD-001", "S-001", 10, 12, 2.0, JobStatus::WAITING, "2026-06-12 09:00:00", "", ""});
    EXPECT_FALSE(repo_->findRunning().has_value());
}

// T19: findWaiting returns WAITING jobs sorted by enqueuedAt ascending
TEST_F(ProductionJobRepositoryTest, FindWaitingReturnsSortedByEnqueuedAt) {
    repo_->save({"JOB-001", "ORD-001", "S-001", 10, 12, 2.0, JobStatus::WAITING, "2026-06-12 09:10:00", "",                   ""});
    repo_->save({"JOB-002", "ORD-002", "S-001", 20, 25, 3.0, JobStatus::RUNNING, "2026-06-12 09:05:00", "2026-06-12 09:06:00", ""});
    repo_->save({"JOB-003", "ORD-003", "S-002", 15, 18, 2.5, JobStatus::WAITING, "2026-06-12 09:00:00", "",                   ""});
    auto waiting = repo_->findWaiting();
    ASSERT_EQ(waiting.size(), 2u);
    EXPECT_EQ(waiting[0].jobId, "JOB-003");
    EXPECT_EQ(waiting[1].jobId, "JOB-001");
}
