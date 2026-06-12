#include "Order.h"

JsonValue Order::toJson() const {
    std::map<std::string, JsonValue> obj;
    obj["orderId"]      = JsonValue(orderId);
    obj["sampleId"]     = JsonValue(sampleId);
    obj["customerName"] = JsonValue(customerName);
    obj["quantity"]     = JsonValue(quantity);
    obj["status"]       = JsonValue(toString(status));
    obj["createdAt"]    = JsonValue(createdAt);
    obj["updatedAt"]    = JsonValue(updatedAt);
    return JsonValue(std::move(obj));
}

Order Order::fromJson(const JsonValue& json) {
    Order o;
    o.orderId      = json.contains("orderId")      ? json["orderId"].asString()                        : "";
    o.sampleId     = json.contains("sampleId")     ? json["sampleId"].asString()                      : "";
    o.customerName = json.contains("customerName") ? json["customerName"].asString()                   : "";
    o.quantity     = json.contains("quantity")     ? json["quantity"].asInt()                          : 0;
    o.status       = json.contains("status")       ? orderStatusFromString(json["status"].asString())  : OrderStatus::RESERVED;
    o.createdAt    = json.contains("createdAt")    ? json["createdAt"].asString()                      : "";
    o.updatedAt    = json.contains("updatedAt")    ? json["updatedAt"].asString()                      : "";
    return o;
}
