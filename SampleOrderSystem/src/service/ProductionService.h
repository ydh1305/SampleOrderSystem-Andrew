#pragma once
#include "repository/SampleRepository.h"
#include "repository/OrderRepository.h"
#include "repository/ProductionJobRepository.h"

class ProductionService {
public:
    ProductionService(SampleRepository& sampleRepo,
                      OrderRepository& orderRepo,
                      ProductionJobRepository& jobRepo);

    void completeCurrentJob();
    void startNextJob();

private:
    SampleRepository&          sampleRepo_;
    OrderRepository&           orderRepo_;
    ProductionJobRepository&   jobRepo_;
};
