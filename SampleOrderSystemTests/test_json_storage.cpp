#include <gmock/gmock.h>
#include <filesystem>
#include "storage/JsonStorage.h"

class JsonStorageTest : public ::testing::Test {
protected:
    void SetUp() override {
        testPath_ = "data/test_storage_" + std::to_string(counter_++) + ".json";
        storage_  = std::make_unique<JsonStorage>(testPath_);
    }
    void TearDown() override {
        std::filesystem::remove(testPath_);
    }
    std::string testPath_;
    std::unique_ptr<JsonStorage> storage_;
    static int counter_;
};
int JsonStorageTest::counter_ = 0;

// T12: loadAll() returns empty vector when file is absent
TEST_F(JsonStorageTest, LoadAllReturnsEmptyWhenFileAbsent) {
    EXPECT_TRUE(storage_->loadAll().empty());
}

// T13: saveAll + loadAll round-trip
TEST_F(JsonStorageTest, RoundTrip) {
    std::map<std::string, JsonValue> obj;
    obj["id"]   = JsonValue(1);
    obj["name"] = JsonValue(std::string("test"));
    std::vector<JsonValue> items;
    items.emplace_back(std::move(obj));

    storage_->saveAll(items);
    auto loaded = storage_->loadAll();

    ASSERT_EQ(loaded.size(), 1u);
    EXPECT_EQ(loaded[0]["id"].asInt(), 1);
    EXPECT_EQ(loaded[0]["name"].asString(), "test");
}

// T14: saveAll immediately creates the file
TEST_F(JsonStorageTest, SaveAllCreatesFile) {
    storage_->saveAll({});
    EXPECT_TRUE(std::filesystem::exists(testPath_));
}

// T15: saveAll creates data/ directory automatically if absent
TEST_F(JsonStorageTest, SaveAllCreatesDataDirectory) {
    JsonStorage s("data/nested/test_dir_create.json");
    s.saveAll({});
    EXPECT_TRUE(std::filesystem::exists("data/nested"));
    std::filesystem::remove("data/nested/test_dir_create.json");
    std::filesystem::remove("data/nested");
}
