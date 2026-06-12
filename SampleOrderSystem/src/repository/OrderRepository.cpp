#include "OrderRepository.h"
#include <algorithm>
#include <stdexcept>
#include <iomanip>
#include <sstream>

OrderRepository::OrderRepository(const std::string& filePath)
    : storage_(filePath) {}

std::vector<Order> OrderRepository::findAll() const {
    auto raw = storage_.loadAll();
    std::vector<Order> result;
    result.reserve(raw.size());
    for (const auto& v : raw)
        result.push_back(Order::fromJson(v));
    return result;
}

std::optional<Order> OrderRepository::findById(const std::string& orderId) const {
    for (const auto& o : findAll())
        if (o.orderId == orderId) return o;
    return std::nullopt;
}

std::vector<Order> OrderRepository::findByStatus(OrderStatus status) const {
    std::vector<Order> result;
    for (const auto& o : findAll())
        if (o.status == status) result.push_back(o);
    return result;
}

void OrderRepository::save(const Order& order) {
    if (findById(order.orderId).has_value())
        throw std::runtime_error("Duplicate orderId: " + order.orderId);
    auto all = findAll();
    all.push_back(order);
    std::vector<JsonValue> raw;
    for (const auto& o : all) raw.push_back(o.toJson());
    storage_.saveAll(raw);
}

void OrderRepository::update(const Order& order) {
    auto all = findAll();
    auto it = std::find_if(all.begin(), all.end(),
        [&](const Order& o) { return o.orderId == order.orderId; });
    if (it == all.end())
        throw std::runtime_error("Order not found: " + order.orderId);
    *it = order;
    std::vector<JsonValue> raw;
    for (const auto& o : all) raw.push_back(o.toJson());
    storage_.saveAll(raw);
}

int OrderRepository::getNextSequence(const std::string& date) const {
    // prefix: "ORD-YYYYMMDD-"
    std::string prefix = "ORD-" + date + "-";
    int maxSeq = 0;
    for (const auto& o : findAll()) {
        if (o.orderId.size() > prefix.size() &&
            o.orderId.substr(0, prefix.size()) == prefix) {
            try {
                int seq = std::stoi(o.orderId.substr(prefix.size()));
                maxSeq = std::max(maxSeq, seq);
            } catch (...) {}
        }
    }
    return maxSeq + 1;
}
