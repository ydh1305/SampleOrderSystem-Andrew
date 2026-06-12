#include <gmock/gmock.h>
#include <filesystem>
#include "repository/SampleRepository.h"

class SampleRepositoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        testPath_ = "data/test_sample_" + std::to_string(counter_++) + ".json";
        repo_ = std::make_unique<SampleRepository>(testPath_);
    }
    void TearDown() override { std::filesystem::remove(testPath_); }
    std::string testPath_;
    std::unique_ptr<SampleRepository> repo_;
    static int counter_;
};
int SampleRepositoryTest::counter_ = 0;

// T1: findAll returns empty initially
TEST_F(SampleRepositoryTest, FindAllReturnsEmptyInitially) {
    EXPECT_TRUE(repo_->findAll().empty());
}

// T2: save then findById returns saved sample
TEST_F(SampleRepositoryTest, SaveAndFindById) {
    Sample s{"S-001", "Wafer-8inch", 0.8, 0.92, 100};
    repo_->save(s);
    auto found = repo_->findById("S-001");
    ASSERT_TRUE(found.has_value());
    EXPECT_EQ(found->name,  "Wafer-8inch");
    EXPECT_EQ(found->stock, 100);
}

// T3: update changes a field
TEST_F(SampleRepositoryTest, UpdateChangesStock) {
    Sample s{"S-001", "Wafer-8inch", 0.8, 0.92, 100};
    repo_->save(s);
    s.stock = 999;
    repo_->update(s);
    EXPECT_EQ(repo_->findById("S-001")->stock, 999);
}

// T4: save with duplicate id throws
TEST_F(SampleRepositoryTest, SaveDuplicateIdThrows) {
    Sample s{"S-001", "Wafer-8inch", 0.8, 0.92, 100};
    repo_->save(s);
    EXPECT_THROW(repo_->save(s), std::runtime_error);
}

// T5: generateNextId returns S-001 when empty
TEST_F(SampleRepositoryTest, GenerateNextIdWhenEmpty) {
    EXPECT_EQ(repo_->generateNextId(), "S-001");
}

// T6: generateNextId increments past existing max
TEST_F(SampleRepositoryTest, GenerateNextIdIncrementsCorrectly) {
    repo_->save({"S-001", "A", 0.5, 0.9, 10});
    repo_->save({"S-002", "B", 0.5, 0.9, 10});
    repo_->save({"S-003", "C", 0.5, 0.9, 10});
    EXPECT_EQ(repo_->generateNextId(), "S-004");
}
