#include "OrderApprovalService.h"
#include "model/ProductionCalculator.h"
#include "view/ConsoleUtil.h"
#include <stdexcept>

OrderApprovalService::OrderApprovalService(ISampleRepository& sampleRepo,
                                           IOrderRepository& orderRepo,
                                           IProductionJobRepository& jobRepo)
    : sampleRepo_(sampleRepo), orderRepo_(orderRepo), jobRepo_(jobRepo) {}

void OrderApprovalService::approve(const std::string& orderId) {
    auto orderOpt = orderRepo_.findById(orderId);
    if (!orderOpt.has_value())
        throw std::runtime_error("Order not found: " + orderId);

    Order order = *orderOpt;
    auto sampleOpt = sampleRepo_.findById(order.sampleId);
    if (!sampleOpt.has_value())
        throw std::runtime_error("Sample not found: " + order.sampleId);

    Sample sample = *sampleOpt;

    if (sample.stock >= order.quantity) {
        sample.stock -= order.quantity;
        sampleRepo_.update(sample);
        order.status    = OrderStatus::CONFIRMED;
        order.updatedAt = ConsoleUtil::getCurrentDateTime();
        orderRepo_.update(order);
    } else {
        int shortage   = order.quantity - sample.stock;
        int actualProd = ProductionCalculator::calculateActualProduction(shortage, sample.yieldRate);
        double totalTime = ProductionCalculator::calculateTotalTime(sample.avgProductionTime, actualProd);

        ProductionJob job;
        job.jobId      = jobRepo_.generateNextId();
        job.orderId    = orderId;
        job.sampleId   = order.sampleId;
        job.shortage   = shortage;
        job.actualProd = actualProd;
        job.totalTime  = totalTime;
        job.enqueuedAt = ConsoleUtil::getCurrentDateTime();
        if (!jobRepo_.findRunning().has_value()) {
            job.status    = JobStatus::RUNNING;
            job.startedAt = ConsoleUtil::getCurrentDateTime();
        } else {
            job.status = JobStatus::WAITING;
        }
        jobRepo_.save(job);

        order.status    = OrderStatus::PRODUCING;
        order.updatedAt = ConsoleUtil::getCurrentDateTime();
        orderRepo_.update(order);
    }
}

void OrderApprovalService::reject(const std::string& orderId) {
    auto orderOpt = orderRepo_.findById(orderId);
    if (!orderOpt.has_value())
        throw std::runtime_error("Order not found: " + orderId);

    Order order = *orderOpt;
    order.status    = OrderStatus::REJECTED;
    order.updatedAt = ConsoleUtil::getCurrentDateTime();
    orderRepo_.update(order);
}
