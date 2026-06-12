#pragma once
#include <string>
#include <vector>
#include <optional>
#include "model/ProductionJob.h"
#include "model/JobStatus.h"
#include "storage/JsonStorage.h"
#include "IProductionJobRepository.h"

class ProductionJobRepository : public IProductionJobRepository {
public:
    explicit ProductionJobRepository(const std::string& filePath = "data/production_jobs.json");

    std::vector<ProductionJob>   findAll() const;
    std::optional<ProductionJob> findById(const std::string& jobId) const;
    std::vector<ProductionJob>   findByStatus(JobStatus status) const;
    void                         save(const ProductionJob& job);
    void                         update(const ProductionJob& job);
    std::string                  generateNextId() const;
    std::optional<ProductionJob> findRunning() const;
    std::vector<ProductionJob>   findWaiting() const;

private:
    JsonStorage storage_;
};
