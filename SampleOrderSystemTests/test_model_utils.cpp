#include <gmock/gmock.h>
#include "model/OrderIdGenerator.h"

// T1: generate returns formatted order ID
TEST(OrderIdGeneratorTest, GenerateFormatsCorrectly) {
    EXPECT_EQ(OrderIdGenerator::generate("20260612", 1),    "ORD-20260612-0001");
    EXPECT_EQ(OrderIdGenerator::generate("20260612", 42),   "ORD-20260612-0042");
    EXPECT_EQ(OrderIdGenerator::generate("20260612", 9999), "ORD-20260612-9999");
}

// T2: generate pads sequence to 4 digits
TEST(OrderIdGeneratorTest, GeneratePadsSequenceTo4Digits) {
    std::string id = OrderIdGenerator::generate("20260612", 1);
    EXPECT_EQ(id, "ORD-20260612-0001");
    EXPECT_EQ(id.size(), 17u);
}

// T3: getDatePart extracts date from order ID
TEST(OrderIdGeneratorTest, GetDatePartExtractsDate) {
    EXPECT_EQ(OrderIdGenerator::getDatePart("ORD-20260612-0001"), "20260612");
}

// T4: getSequencePart extracts sequence number
TEST(OrderIdGeneratorTest, GetSequencePartExtractsSequence) {
    EXPECT_EQ(OrderIdGenerator::getSequencePart("ORD-20260612-0042"), 42);
    EXPECT_EQ(OrderIdGenerator::getSequencePart("ORD-20260612-0001"), 1);
}

// T5: isValidOrderId returns true for valid IDs
TEST(OrderIdGeneratorTest, IsValidOrderIdReturnsTrueForValid) {
    EXPECT_TRUE(OrderIdGenerator::isValidOrderId("ORD-20260612-0001"));
    EXPECT_FALSE(OrderIdGenerator::isValidOrderId("INVALID"));
    EXPECT_FALSE(OrderIdGenerator::isValidOrderId("ORD-2026-0001"));
}

#include <filesystem>
#include "model/ProductionQueue.h"
#include "repository/ProductionJobRepository.h"

class ProductionQueueTest : public ::testing::Test {
protected:
    void SetUp() override {
        jobPath_ = "data/test_pq_" + std::to_string(counter_++) + ".json";
        repo_ = std::make_unique<ProductionJobRepository>(jobPath_);
        queue_ = std::make_unique<ProductionQueue>(*repo_);
    }
    void TearDown() override { std::filesystem::remove(jobPath_); }
    std::string jobPath_;
    std::unique_ptr<ProductionJobRepository> repo_;
    std::unique_ptr<ProductionQueue> queue_;
    static int counter_;
};
int ProductionQueueTest::counter_ = 0;

// T6: isEmpty returns true when no jobs
TEST_F(ProductionQueueTest, IsEmptyWhenNoJobs) {
    EXPECT_TRUE(queue_->isEmpty());
}

// T7: hasRunning returns false when no RUNNING job
TEST_F(ProductionQueueTest, HasRunningReturnsFalseWhenEmpty) {
    EXPECT_FALSE(queue_->hasRunning());
}

// T8: enqueue adds a WAITING job, size increases
TEST_F(ProductionQueueTest, EnqueueAddsWaitingJob) {
    ProductionJob job{"JOB-001", "ORD-001", "S-001", 50, 61, 48.8,
                      JobStatus::WAITING, "2026-06-12 09:00:00", "", ""};
    queue_->enqueue(job);
    EXPECT_EQ(queue_->waitingCount(), 1);
}

// T9: peekNext returns the oldest WAITING job (FIFO)
TEST_F(ProductionQueueTest, PeekNextReturnsOldest) {
    repo_->save({"JOB-001", "ORD-001", "S-001", 50, 61, 48.8, JobStatus::WAITING, "2026-06-12 09:10:00", "", ""});
    repo_->save({"JOB-002", "ORD-002", "S-001", 80, 97, 77.6, JobStatus::WAITING, "2026-06-12 09:00:00", "", ""});
    auto next = queue_->peekNext();
    ASSERT_TRUE(next.has_value());
    EXPECT_EQ(next->jobId, "JOB-002");  // earlier enqueuedAt
}

// T10: getRunning returns nullopt when no RUNNING job
TEST_F(ProductionQueueTest, GetRunningReturnsNulloptWhenNone) {
    EXPECT_FALSE(queue_->getRunning().has_value());
}
