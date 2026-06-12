#include <gmock/gmock.h>
#include <filesystem>
#include "util/DummyDataLoader.h"
#include "repository/SampleRepository.h"
#include "repository/OrderRepository.h"
#include "repository/ProductionJobRepository.h"

class DummyDataLoaderTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::string s = std::to_string(counter_++);
        samplePath_ = "data/test_dl_s_" + s + ".json";
        orderPath_  = "data/test_dl_o_" + s + ".json";
        jobPath_    = "data/test_dl_j_" + s + ".json";
        sampleRepo_ = std::make_unique<SampleRepository>(samplePath_);
        orderRepo_  = std::make_unique<OrderRepository>(orderPath_);
        jobRepo_    = std::make_unique<ProductionJobRepository>(jobPath_);
    }
    void TearDown() override {
        std::filesystem::remove(samplePath_);
        std::filesystem::remove(orderPath_);
        std::filesystem::remove(jobPath_);
    }

    std::unique_ptr<SampleRepository>        sampleRepo_;
    std::unique_ptr<OrderRepository>         orderRepo_;
    std::unique_ptr<ProductionJobRepository> jobRepo_;
    std::string samplePath_, orderPath_, jobPath_;
    static int counter_;
};
int DummyDataLoaderTest::counter_ = 0;

// DL1: load() 호출 후 시료 4개 생성
TEST_F(DummyDataLoaderTest, LoadCreatesFourSamples) {
    DummyDataLoader::load(*sampleRepo_, *orderRepo_, *jobRepo_);
    EXPECT_EQ(sampleRepo_->findAll().size(), 4u);
}

// DL2: S-001 실리콘 웨이퍼-8인치 stock=480 생성
TEST_F(DummyDataLoaderTest, FirstSampleIsS001WithStock480) {
    DummyDataLoader::load(*sampleRepo_, *orderRepo_, *jobRepo_);
    auto opt = sampleRepo_->findById("S-001");
    ASSERT_TRUE(opt.has_value());
    EXPECT_EQ(opt->stock, 480);
    EXPECT_DOUBLE_EQ(opt->yieldRate, 0.92);
}

// DL3: S-004 산화막 웨이퍼 stock=0 (고갈 상태 시연용)
TEST_F(DummyDataLoaderTest, S004HasZeroStock) {
    DummyDataLoader::load(*sampleRepo_, *orderRepo_, *jobRepo_);
    auto opt = sampleRepo_->findById("S-004");
    ASSERT_TRUE(opt.has_value());
    EXPECT_EQ(opt->stock, 0);
}

// DL4: 주문 10개 이상 생성
TEST_F(DummyDataLoaderTest, LoadCreatesAtLeastTenOrders) {
    DummyDataLoader::load(*sampleRepo_, *orderRepo_, *jobRepo_);
    EXPECT_GE(orderRepo_->findAll().size(), 10u);
}

// DL5: RESERVED / CONFIRMED / PRODUCING 상태 주문이 각각 1개 이상
TEST_F(DummyDataLoaderTest, OrdersHaveVariousStatuses) {
    DummyDataLoader::load(*sampleRepo_, *orderRepo_, *jobRepo_);
    auto orders = orderRepo_->findAll();
    int reserved = 0, confirmed = 0, producing = 0;
    for (const auto& o : orders) {
        if (o.status == OrderStatus::RESERVED)  reserved++;
        if (o.status == OrderStatus::CONFIRMED) confirmed++;
        if (o.status == OrderStatus::PRODUCING) producing++;
    }
    EXPECT_GE(reserved,  1);
    EXPECT_GE(confirmed, 1);
    EXPECT_GE(producing, 1);
}
