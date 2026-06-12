#pragma once
#include <string>
#include "IModel.h"
#include "OrderStatus.h"

struct Order : public IModel {
    std::string orderId;
    std::string sampleId;
    std::string customerName;
    int         quantity  = 0;
    OrderStatus status    = OrderStatus::RESERVED;
    std::string createdAt;
    std::string updatedAt;

    JsonValue toJson() const override;
    void fromJson(const JsonValue& json) override;
};
