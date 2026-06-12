#include "Sample.h"

JsonValue Sample::toJson() const {
    std::map<std::string, JsonValue> obj;
    obj["sampleId"]          = JsonValue(sampleId);
    obj["name"]              = JsonValue(name);
    obj["avgProductionTime"] = JsonValue(avgProductionTime);
    obj["yieldRate"]         = JsonValue(yieldRate);
    obj["stock"]             = JsonValue(stock);
    return JsonValue(std::move(obj));
}

Sample Sample::fromJson(const JsonValue& json) {
    Sample s;
    s.sampleId          = json["sampleId"].asString();
    s.name              = json["name"].asString();
    s.avgProductionTime = json["avgProductionTime"].asDouble();
    s.yieldRate         = json["yieldRate"].asDouble();
    s.stock             = json["stock"].asInt();
    return s;
}
