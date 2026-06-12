#include <gmock/gmock.h>
#include <filesystem>
#include "repository/OrderRepository.h"

class OrderRepositoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        testPath_ = "data/test_order_" + std::to_string(counter_++) + ".json";
        repo_ = std::make_unique<OrderRepository>(testPath_);
    }
    void TearDown() override { std::filesystem::remove(testPath_); }
    std::string testPath_;
    std::unique_ptr<OrderRepository> repo_;
    static int counter_;
};
int OrderRepositoryTest::counter_ = 0;

// T7: findAll returns empty initially
TEST_F(OrderRepositoryTest, FindAllReturnsEmptyInitially) {
    EXPECT_TRUE(repo_->findAll().empty());
}

// T8: save then findById returns saved order
TEST_F(OrderRepositoryTest, SaveAndFindById) {
    Order o{"ORD-20260612-0001", "S-001", "Alice", 50, OrderStatus::RESERVED, "2026-06-12", ""};
    repo_->save(o);
    auto found = repo_->findById("ORD-20260612-0001");
    ASSERT_TRUE(found.has_value());
    EXPECT_EQ(found->customerName, "Alice");
    EXPECT_EQ(found->quantity, 50);
}

// T9: update changes status
TEST_F(OrderRepositoryTest, UpdateChangesStatus) {
    Order o{"ORD-20260612-0001", "S-001", "Alice", 50, OrderStatus::RESERVED, "2026-06-12", ""};
    repo_->save(o);
    o.status = OrderStatus::CONFIRMED;
    repo_->update(o);
    EXPECT_EQ(repo_->findById("ORD-20260612-0001")->status, OrderStatus::CONFIRMED);
}

// T10: findByStatus returns only matching orders
TEST_F(OrderRepositoryTest, FindByStatusReturnsMatching) {
    repo_->save({"ORD-20260612-0001", "S-001", "Alice", 10, OrderStatus::RESERVED,  "2026-06-12", ""});
    repo_->save({"ORD-20260612-0002", "S-001", "Bob",   20, OrderStatus::CONFIRMED, "2026-06-12", ""});
    repo_->save({"ORD-20260612-0003", "S-002", "Carol", 30, OrderStatus::RESERVED,  "2026-06-12", ""});

    auto reserved = repo_->findByStatus(OrderStatus::RESERVED);
    ASSERT_EQ(reserved.size(), 2u);
    auto confirmed = repo_->findByStatus(OrderStatus::CONFIRMED);
    ASSERT_EQ(confirmed.size(), 1u);
    EXPECT_EQ(confirmed[0].customerName, "Bob");
}

// T11: getNextSequence returns 1 when empty, increments correctly
TEST_F(OrderRepositoryTest, GetNextSequenceIncrements) {
    EXPECT_EQ(repo_->getNextSequence("20260612"), 1);
    repo_->save({"ORD-20260612-0001", "S-001", "Alice", 10, OrderStatus::RESERVED, "2026-06-12", ""});
    EXPECT_EQ(repo_->getNextSequence("20260612"), 2);
    repo_->save({"ORD-20260612-0002", "S-001", "Bob", 20, OrderStatus::RESERVED, "2026-06-12", ""});
    EXPECT_EQ(repo_->getNextSequence("20260612"), 3);
}
