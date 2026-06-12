#include "ProductionQueue.h"

ProductionQueue::ProductionQueue(ProductionJobRepository& repo) : repo_(repo) {}

void ProductionQueue::enqueue(const ProductionJob& job) {
    repo_.save(job);
}

std::optional<ProductionJob> ProductionQueue::peekNext() const {
    auto waiting = repo_.findWaiting();
    if (waiting.empty()) return std::nullopt;
    return waiting.front();
}

std::optional<ProductionJob> ProductionQueue::getRunning() const {
    return repo_.findRunning();
}

bool ProductionQueue::isEmpty() const {
    return repo_.findWaiting().empty();
}

bool ProductionQueue::hasRunning() const {
    return repo_.findRunning().has_value();
}

int ProductionQueue::waitingCount() const {
    return static_cast<int>(repo_.findWaiting().size());
}
