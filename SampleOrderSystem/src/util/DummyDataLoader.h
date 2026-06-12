#pragma once
#include "ISampleRepository.h"
#include "IOrderRepository.h"
#include "IProductionJobRepository.h"

class DummyDataLoader {
public:
    static void load(ISampleRepository&        sampleRepo,
                     IOrderRepository&         orderRepo,
                     IProductionJobRepository& jobRepo);
};
