#pragma once
#include <iostream>
#include <string>
#include "repository/SampleRepository.h"
#include "repository/OrderRepository.h"
#include "repository/ProductionJobRepository.h"

class MainController {
public:
    explicit MainController(const std::string& samplePath = "data/samples.json",
                            const std::string& orderPath  = "data/orders.json",
                            const std::string& jobPath    = "data/production_jobs.json",
                            std::ostream& out = std::cout,
                            std::istream& in  = std::cin);

    void run();

private:
    SampleRepository        sampleRepo_;
    OrderRepository         orderRepo_;
    ProductionJobRepository jobRepo_;
    std::ostream& out_;
    std::istream& in_;

    void showMainMenu();
};
