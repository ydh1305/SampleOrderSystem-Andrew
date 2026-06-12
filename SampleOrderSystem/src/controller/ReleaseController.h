#pragma once
#include <iostream>
#include <string>
#include "IOrderRepository.h"

class ReleaseController {
public:
    ReleaseController(IOrderRepository& orderRepo,
                      std::ostream& out = std::cout,
                      std::istream& in  = std::cin);

    void processRelease();
    void run();

private:
    IOrderRepository& orderRepo_;
    std::ostream& out_;
    std::istream& in_;
};
