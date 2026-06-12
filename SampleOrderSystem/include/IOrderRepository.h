#pragma once
#include <vector>
#include <string>
#include <optional>
#include "Order.h"

class IOrderRepository {
public:
    virtual ~IOrderRepository() = default;

    virtual void                  save(const Order& order)                     = 0;
    virtual std::optional<Order>  findById(const std::string& id) const        = 0;
    virtual std::vector<Order>    findAll() const                              = 0;
    virtual std::vector<Order>    findBySampleId(const std::string& sampleId) const = 0;
    virtual void                  update(const Order& order)                   = 0;
    virtual void                  remove(const std::string& id)                = 0;
};
