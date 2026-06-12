#pragma once
#include <string>
#include <vector>
#include <optional>
#include "model/Order.h"
#include "model/OrderStatus.h"
#include "storage/JsonStorage.h"
#include "IOrderRepository.h"

class OrderRepository : public IOrderRepository {
public:
    explicit OrderRepository(const std::string& filePath = "data/orders.json");

    std::vector<Order>   findAll() const;
    std::optional<Order> findById(const std::string& orderId) const;
    std::vector<Order>   findByStatus(OrderStatus status) const;
    void                 save(const Order& order);
    void                 update(const Order& order);
    int                  getNextSequence(const std::string& date) const;

private:
    JsonStorage storage_;
};
