#pragma once
#include "JsonValue.h"
#include <string>
#include <vector>

class JsonStorage {
public:
    explicit JsonStorage(const std::string& filePath);

    std::vector<JsonValue> loadAll();
    void saveAll(const std::vector<JsonValue>& items);

private:
    std::string filePath_;
    void ensureDirectoryExists() const;
};
