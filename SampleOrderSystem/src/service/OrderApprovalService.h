#pragma once
#include <string>
#include "repository/SampleRepository.h"
#include "repository/OrderRepository.h"
#include "repository/ProductionJobRepository.h"

class OrderApprovalService {
public:
    OrderApprovalService(SampleRepository& sampleRepo,
                         OrderRepository& orderRepo,
                         ProductionJobRepository& jobRepo);

    void approve(const std::string& orderId);
    void reject(const std::string& orderId);

private:
    SampleRepository&          sampleRepo_;
    OrderRepository&           orderRepo_;
    ProductionJobRepository&   jobRepo_;
};
