#pragma once
#include <iostream>
#include <string>
#include "ISampleRepository.h"
#include "IOrderRepository.h"

class MonitoringController {
public:
    MonitoringController(ISampleRepository& sampleRepo,
                         IOrderRepository& orderRepo,
                         std::ostream& out = std::cout,
                         std::istream& in  = std::cin);

    void showOrderCounts();
    void showStockStatus();
    void run();

private:
    ISampleRepository& sampleRepo_;
    IOrderRepository&  orderRepo_;
    std::ostream& out_;
    std::istream& in_;
};
