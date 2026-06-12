#pragma once
#include "JsonValue.h"
#include <string>

class JsonParser {
public:
    static JsonValue parse(const std::string& input);

private:
    explicit JsonParser(const std::string& input);

    JsonValue parseValue();
    JsonValue parseObject();
    JsonValue parseArray();
    JsonValue parseString();
    JsonValue parseNumber();
    JsonValue parseLiteral();
    void      skipWhitespace();
    char      peek() const;
    char      consume();

    const std::string& input_;
    size_t pos_ = 0;
};
