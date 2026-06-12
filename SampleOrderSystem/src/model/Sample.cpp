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
    s.sampleId          = json.contains("sampleId")          ? json["sampleId"].asString()          : "";
    s.name              = json.contains("name")              ? json["name"].asString()              : "";
    s.avgProductionTime = json.contains("avgProductionTime") ? json["avgProductionTime"].asDouble() : 0.0;
    s.yieldRate         = json.contains("yieldRate")         ? json["yieldRate"].asDouble()         : 0.0;
    s.stock             = json.contains("stock")             ? json["stock"].asInt()                : 0;
    return s;
}
