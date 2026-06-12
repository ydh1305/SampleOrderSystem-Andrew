#pragma once
#include <vector>
#include <string>
#include <optional>
#include "ProductionJob.h"

class IProductionJobRepository {
public:
    virtual ~IProductionJobRepository() = default;

    virtual void                         save(const ProductionJob& job)                = 0;
    virtual std::optional<ProductionJob> findById(const std::string& id) const         = 0;
    virtual std::vector<ProductionJob>   findAll() const                               = 0;
    virtual std::vector<ProductionJob>   findByOrderId(const std::string& orderId) const = 0;
    virtual void                         update(const ProductionJob& job)              = 0;
    virtual void                         remove(const std::string& id)                 = 0;
};
