#pragma once
#include <iostream>
#include <map>
#include <vector>
#include "model/Sample.h"
#include "model/OrderStatus.h"

struct SampleStockInfo {
    Sample      sample;
    int         pendingQty;
    std::string stockStatus;
};

class MonitoringView {
public:
    explicit MonitoringView(std::ostream& out = std::cout, std::istream& in = std::cin);

    void showSubMenu();
    void showOrderSummary(const std::map<OrderStatus, int>& counts);
    void showStockStatus(const std::vector<SampleStockInfo>& info);

    int readMenuChoice();

private:
    std::ostream& out_;
    std::istream& in_;
};
