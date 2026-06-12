#pragma once
#include <optional>
#include "repository/ProductionJobRepository.h"
#include "model/ProductionJob.h"

class ProductionQueue {
public:
    explicit ProductionQueue(ProductionJobRepository& repo);

    void                         enqueue(const ProductionJob& job);
    std::optional<ProductionJob> peekNext() const;
    std::optional<ProductionJob> getRunning() const;
    bool                         isEmpty() const;
    bool                         hasRunning() const;
    int                          waitingCount() const;

private:
    ProductionJobRepository& repo_;
};
