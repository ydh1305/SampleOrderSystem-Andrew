#include "JsonValue.h"
#include <stdexcept>
#include <sstream>

JsonValue::JsonValue()                                        : type_(JsonType::Null)   {}
JsonValue::JsonValue(bool v)                                  : type_(JsonType::Bool),   boolVal_(v)   {}
JsonValue::JsonValue(int v)                                   : type_(JsonType::Int),    intVal_(v)    {}
JsonValue::JsonValue(double v)                                : type_(JsonType::Double), doubleVal_(v) {}
JsonValue::JsonValue(const std::string& v)                    : type_(JsonType::String), strVal_(v)    {}
JsonValue::JsonValue(std::vector<JsonValue> v)                : type_(JsonType::Array),  arrVal_(std::move(v)) {}
JsonValue::JsonValue(std::map<std::string, JsonValue> v)      : type_(JsonType::Object), objVal_(std::move(v)) {}

JsonType JsonValue::type() const { return type_; }

bool        JsonValue::asBool()   const { if (type_ != JsonType::Bool)   throw std::runtime_error("not Bool");   return boolVal_;   }
int         JsonValue::asInt()    const { if (type_ != JsonType::Int)    throw std::runtime_error("not Int");    return intVal_;    }
double      JsonValue::asDouble() const { if (type_ != JsonType::Double) throw std::runtime_error("not Double"); return doubleVal_; }
const std::string& JsonValue::asString() const { if (type_ != JsonType::String) throw std::runtime_error("not String"); return strVal_; }
const std::vector<JsonValue>&           JsonValue::asArray()  const { if (type_ != JsonType::Array)  throw std::runtime_error("not Array");  return arrVal_; }
const std::map<std::string, JsonValue>& JsonValue::asObject() const { if (type_ != JsonType::Object) throw std::runtime_error("not Object"); return objVal_; }

JsonValue& JsonValue::operator[](const std::string& key) {
    if (type_ != JsonType::Object) throw std::runtime_error("not Object");
    return objVal_[key];
}
const JsonValue& JsonValue::operator[](const std::string& key) const {
    if (type_ != JsonType::Object) throw std::runtime_error("not Object");
    return objVal_.at(key);
}
bool JsonValue::contains(const std::string& key) const {
    return type_ == JsonType::Object && objVal_.count(key) > 0;
}

namespace {
std::string escapeString(const std::string& s) {
    std::string out = "\"";
    for (char c : s) {
        switch (c) {
            case '"':  out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n";  break;
            case '\t': out += "\\t";  break;
            default:   out += c;
        }
    }
    return out + "\"";
}

std::string serializeImpl(const JsonValue& v, int indent, int depth) {
    std::string pad(depth * indent, ' ');
    std::string padInner((depth + 1) * indent, ' ');

    switch (v.type()) {
        case JsonType::Null:   return "null";
        case JsonType::Bool:   return v.asBool() ? "true" : "false";
        case JsonType::Int:    return std::to_string(v.asInt());
        case JsonType::Double: {
            std::ostringstream oss;
            oss << v.asDouble();
            return oss.str();
        }
        case JsonType::String: return escapeString(v.asString());
        case JsonType::Array: {
            const auto& arr = v.asArray();
            if (arr.empty()) return "[]";
            std::string out = "[\n";
            for (size_t i = 0; i < arr.size(); ++i) {
                out += padInner + serializeImpl(arr[i], indent, depth + 1);
                if (i + 1 < arr.size()) out += ",";
                out += "\n";
            }
            return out + pad + "]";
        }
        case JsonType::Object: {
            const auto& obj = v.asObject();
            if (obj.empty()) return "{}";
            std::string out = "{\n";
            size_t i = 0;
            for (const auto& [k, val] : obj) {
                out += padInner + escapeString(k) + ": " + serializeImpl(val, indent, depth + 1);
                if (++i < obj.size()) out += ",";
                out += "\n";
            }
            return out + pad + "}";
        }
    }
    return "null";
}
} // namespace

std::string JsonValue::serialize(int indent) const {
    return serializeImpl(*this, indent, 0);
}
