#include "ProductionJobRepository.h"
#include <algorithm>
#include <stdexcept>
#include <iomanip>
#include <sstream>

ProductionJobRepository::ProductionJobRepository(const std::string& filePath)
    : storage_(filePath) {}

std::vector<ProductionJob> ProductionJobRepository::findAll() const {
    auto raw = storage_.loadAll();
    std::vector<ProductionJob> result;
    result.reserve(raw.size());
    for (const auto& v : raw)
        result.push_back(ProductionJob::fromJson(v));
    return result;
}

std::optional<ProductionJob> ProductionJobRepository::findById(const std::string& jobId) const {
    for (const auto& j : findAll())
        if (j.jobId == jobId) return j;
    return std::nullopt;
}

std::vector<ProductionJob> ProductionJobRepository::findByStatus(JobStatus status) const {
    std::vector<ProductionJob> result;
    for (const auto& j : findAll())
        if (j.status == status) result.push_back(j);
    return result;
}

void ProductionJobRepository::save(const ProductionJob& job) {
    if (findById(job.jobId).has_value())
        throw std::runtime_error("Duplicate jobId: " + job.jobId);
    auto all = findAll();
    all.push_back(job);
    std::vector<JsonValue> raw;
    for (const auto& j : all) raw.push_back(j.toJson());
    storage_.saveAll(raw);
}

void ProductionJobRepository::update(const ProductionJob& job) {
    auto all = findAll();
    auto it = std::find_if(all.begin(), all.end(),
        [&](const ProductionJob& j) { return j.jobId == job.jobId; });
    if (it == all.end())
        throw std::runtime_error("ProductionJob not found: " + job.jobId);
    *it = job;
    std::vector<JsonValue> raw;
    for (const auto& j : all) raw.push_back(j.toJson());
    storage_.saveAll(raw);
}

std::string ProductionJobRepository::generateNextId() const {
    int maxNum = 0;
    for (const auto& j : findAll()) {
        if (j.jobId.size() > 4 && j.jobId.substr(0, 4) == "JOB-") {
            try { maxNum = std::max(maxNum, std::stoi(j.jobId.substr(4))); }
            catch (...) {}
        }
    }
    std::ostringstream ss;
    ss << "JOB-" << std::setw(3) << std::setfill('0') << (maxNum + 1);
    return ss.str();
}

std::optional<ProductionJob> ProductionJobRepository::findRunning() const {
    for (const auto& j : findAll())
        if (j.status == JobStatus::RUNNING) return j;
    return std::nullopt;
}

std::vector<ProductionJob> ProductionJobRepository::findWaiting() const {
    auto waiting = findByStatus(JobStatus::WAITING);
    std::sort(waiting.begin(), waiting.end(),
        [](const ProductionJob& a, const ProductionJob& b) {
            return a.enqueuedAt < b.enqueuedAt;
        });
    return waiting;
}
