#include "SampleRepository.h"
#include <algorithm>
#include <stdexcept>
#include <iomanip>
#include <sstream>

SampleRepository::SampleRepository(const std::string& filePath)
    : storage_(filePath) {}

std::vector<Sample> SampleRepository::findAll() const {
    auto raw = storage_.loadAll();
    std::vector<Sample> result;
    result.reserve(raw.size());
    for (const auto& v : raw)
        result.push_back(Sample::fromJson(v));
    return result;
}

std::optional<Sample> SampleRepository::findById(const std::string& sampleId) const {
    for (const auto& s : findAll())
        if (s.sampleId == sampleId) return s;
    return std::nullopt;
}

void SampleRepository::save(const Sample& sample) {
    if (findById(sample.sampleId).has_value())
        throw std::runtime_error("Duplicate sampleId: " + sample.sampleId);
    auto all = findAll();
    all.push_back(sample);
    std::vector<JsonValue> raw;
    for (const auto& s : all) raw.push_back(s.toJson());
    storage_.saveAll(raw);
}

void SampleRepository::update(const Sample& sample) {
    auto all = findAll();
    auto it = std::find_if(all.begin(), all.end(),
        [&](const Sample& s) { return s.sampleId == sample.sampleId; });
    if (it == all.end())
        throw std::runtime_error("Sample not found: " + sample.sampleId);
    *it = sample;
    std::vector<JsonValue> raw;
    for (const auto& s : all) raw.push_back(s.toJson());
    storage_.saveAll(raw);
}

std::string SampleRepository::generateNextId() const {
    int maxNum = 0;
    for (const auto& s : findAll()) {
        if (s.sampleId.size() > 2 && s.sampleId.substr(0, 2) == "S-") {
            try { maxNum = std::max(maxNum, std::stoi(s.sampleId.substr(2))); }
            catch (...) {}
        }
    }
    std::ostringstream ss;
    ss << "S-" << std::setw(3) << std::setfill('0') << (maxNum + 1);
    return ss.str();
}
