#pragma once
#include <iostream>
#include <string>
#include "ISampleRepository.h"
#include "IOrderRepository.h"
#include "IProductionJobRepository.h"

class ProductionController {
public:
    ProductionController(ISampleRepository& sampleRepo,
                         IOrderRepository& orderRepo,
                         IProductionJobRepository& jobRepo,
                         std::ostream& out = std::cout,
                         std::istream& in  = std::cin);

    void showProductionLine();
    void completeCurrentJob();
    void run();

private:
    ISampleRepository&        sampleRepo_;
    IOrderRepository&         orderRepo_;
    IProductionJobRepository& jobRepo_;
    std::ostream& out_;
    std::istream& in_;
};
