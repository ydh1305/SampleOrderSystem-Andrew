#pragma once
#include <iostream>
#include <vector>
#include "model/Order.h"

class ReleaseView {
public:
    explicit ReleaseView(std::ostream& out = std::cout, std::istream& in = std::cin);

    void showConfirmedOrders(const std::vector<Order>& orders);
    void showNoConfirmedOrders();
    void showReleaseSuccess(const Order& o);
    void showError(const std::string& msg);

    int  readOrderIndex();
    char readYesNo();

private:
    std::ostream& out_;
    std::istream& in_;
};
