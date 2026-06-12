#pragma once
#include <string>
#include <vector>
#include <map>

enum class JsonType { Null, Bool, Int, Double, String, Array, Object };

class JsonValue {
public:
    JsonValue();
    explicit JsonValue(bool v);
    explicit JsonValue(int v);
    explicit JsonValue(double v);
    explicit JsonValue(const std::string& v);
    explicit JsonValue(std::vector<JsonValue> v);
    explicit JsonValue(std::map<std::string, JsonValue> v);

    JsonType type() const;
    bool     asBool()   const;
    int      asInt()    const;
    double   asDouble() const;
    const std::string&                      asString() const;
    const std::vector<JsonValue>&           asArray()  const;
    const std::map<std::string, JsonValue>& asObject() const;

    JsonValue&       operator[](const std::string& key);
    const JsonValue& operator[](const std::string& key) const;
    bool             contains(const std::string& key) const;

    std::string serialize(int indent = 2) const;

private:
    JsonType type_;
    bool     boolVal_   = false;
    int      intVal_    = 0;
    double   doubleVal_ = 0.0;
    std::string                      strVal_;
    std::vector<JsonValue>           arrVal_;
    std::map<std::string, JsonValue> objVal_;
};
