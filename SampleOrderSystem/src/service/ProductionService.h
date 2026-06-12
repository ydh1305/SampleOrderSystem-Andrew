#pragma once
#include "ISampleRepository.h"
#include "IOrderRepository.h"
#include "IProductionJobRepository.h"

class ProductionService {
public:
    ProductionService(ISampleRepository& sampleRepo,
                      IOrderRepository& orderRepo,
                      IProductionJobRepository& jobRepo);

    void completeCurrentJob();
    void startNextJob();

private:
    ISampleRepository&          sampleRepo_;
    IOrderRepository&           orderRepo_;
    IProductionJobRepository&   jobRepo_;
};
