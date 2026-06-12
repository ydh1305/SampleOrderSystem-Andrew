#include "ProductionService.h"
#include "model/ProductionCalculator.h"
#include "view/ConsoleUtil.h"
#include <stdexcept>

ProductionService::ProductionService(SampleRepository& sampleRepo,
                                     OrderRepository& orderRepo,
                                     ProductionJobRepository& jobRepo)
    : sampleRepo_(sampleRepo), orderRepo_(orderRepo), jobRepo_(jobRepo) {}

void ProductionService::completeCurrentJob() {
    auto running = jobRepo_.findRunning();
    if (!running.has_value())
        throw std::runtime_error("No running production job");

    ProductionJob job = *running;
    auto sampleOpt = sampleRepo_.findById(job.sampleId);
    if (!sampleOpt.has_value())
        throw std::runtime_error("Sample not found: " + job.sampleId);

    Sample sample = *sampleOpt;
    int goodYield = ProductionCalculator::calculateGoodYield(job.actualProd, sample.yieldRate);
    sample.stock += goodYield;
    sampleRepo_.update(sample);

    auto orderOpt = orderRepo_.findById(job.orderId);
    if (orderOpt.has_value()) {
        Order order = *orderOpt;
        order.status    = OrderStatus::CONFIRMED;
        order.updatedAt = ConsoleUtil::getCurrentDateTime();
        orderRepo_.update(order);
    }

    job.status      = JobStatus::DONE;
    job.completedAt = ConsoleUtil::getCurrentDateTime();
    jobRepo_.update(job);

    startNextJob();
}

void ProductionService::startNextJob() {
    auto waiting = jobRepo_.findWaiting();
    if (waiting.empty()) return;

    ProductionJob next = waiting.front();
    next.status    = JobStatus::RUNNING;
    next.startedAt = ConsoleUtil::getCurrentDateTime();
    jobRepo_.update(next);
}
