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
    o.orderId      = json["orderId"].asString();
    o.sampleId     = json["sampleId"].asString();
    o.customerName = json["customerName"].asString();
    o.quantity     = json["quantity"].asInt();
    o.status       = orderStatusFromString(json["status"].asString());
    o.createdAt    = json["createdAt"].asString();
    o.updatedAt    = json["updatedAt"].asString();
    return o;
}
