#include "JsonStorage.h"
#include "JsonParser.h"
#include <fstream>
#include <filesystem>

JsonStorage::JsonStorage(const std::string& filePath) : filePath_(filePath) {}

void JsonStorage::ensureDirectoryExists() const {
    std::filesystem::path p(filePath_);
    if (p.has_parent_path())
        std::filesystem::create_directories(p.parent_path());
}

std::vector<JsonValue> JsonStorage::loadAll() {
    std::ifstream file(filePath_);
    if (!file.is_open()) return {};

    std::string content((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
    if (content.empty()) return {};

    JsonValue root = JsonParser::parse(content);
    if (root.type() != JsonType::Array) return {};
    return root.asArray();
}

void JsonStorage::saveAll(const std::vector<JsonValue>& items) {
    ensureDirectoryExists();
    std::ofstream file(filePath_);
    file << JsonValue(items).serialize(2);
}
