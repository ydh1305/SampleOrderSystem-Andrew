#pragma once
#include <vector>
#include <string>
#include <optional>
#include "Order.h"

class IOrderRepository {
public:
    virtual ~IOrderRepository() = default;

    virtual void                  save(const Order& order)                      = 0;
    virtual std::optional<Order>  findById(const std::string& id) const        = 0;
    virtual std::vector<Order>    findAll() const                              = 0;
    virtual std::vector<Order>    findByStatus(OrderStatus status) const       = 0;
    virtual void                  update(const Order& order)                   = 0;
    virtual int                   getNextSequence(const std::string& date) const = 0;
};
