#pragma once
#include <string>
#include "storage/JsonValue.h"
#include "model/OrderStatus.h"

struct Order {
    std::string orderId;
    std::string sampleId;
    std::string customerName;
    int         quantity  = 0;
    OrderStatus status    = OrderStatus::RESERVED;
    std::string createdAt;
    std::string updatedAt;

    JsonValue    toJson() const;
    static Order fromJson(const JsonValue& json);
};
