#pragma once
#include <string>
#include "ISampleRepository.h"
#include "IOrderRepository.h"
#include "IProductionJobRepository.h"

class OrderApprovalService {
public:
    OrderApprovalService(ISampleRepository& sampleRepo,
                         IOrderRepository& orderRepo,
                         IProductionJobRepository& jobRepo);

    void approve(const std::string& orderId);
    void reject(const std::string& orderId);

private:
    ISampleRepository&          sampleRepo_;
    IOrderRepository&           orderRepo_;
    IProductionJobRepository&   jobRepo_;
};
