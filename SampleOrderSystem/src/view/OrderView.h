#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "model/Order.h"

class OrderView {
public:
    explicit OrderView(std::ostream& out = std::cout, std::istream& in = std::cin);

    void showReservedOrders(const std::vector<Order>& orders);
    void showNoReservedOrders();
    void showOrderList(const std::vector<Order>& orders);
    void showOrderDetail(const Order& o);
    void showOrderCreated(const Order& o);
    void showApprovalResult(const Order& o);
    void showRejectResult(const Order& o);
    void showError(const std::string& msg);

    int         readOrderIndex();
    std::string readSampleId();
    std::string readCustomerName();
    int         readQuantity();
    char        readYesNo();

private:
    std::ostream& out_;
    std::istream& in_;
};
