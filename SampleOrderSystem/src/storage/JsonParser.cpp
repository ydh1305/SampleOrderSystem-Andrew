#include "JsonParser.h"
#include <stdexcept>
#include <cctype>

JsonParser::JsonParser(const std::string& input) : input_(input) {}

JsonValue JsonParser::parse(const std::string& input) {
    JsonParser p(input);
    p.skipWhitespace();
    JsonValue v = p.parseValue();
    p.skipWhitespace();
    return v;
}

char JsonParser::peek() const {
    if (pos_ >= input_.size()) return '\0';
    return input_[pos_];
}

char JsonParser::consume() {
    if (pos_ >= input_.size()) throw std::runtime_error("unexpected end of input");
    return input_[pos_++];
}

void JsonParser::skipWhitespace() {
    while (pos_ < input_.size() && std::isspace(static_cast<unsigned char>(input_[pos_])))
        ++pos_;
}

JsonValue JsonParser::parseValue() {
    skipWhitespace();
    char c = peek();
    if (c == '"')  return parseString();
    if (c == '[')  return parseArray();
    if (c == '{')  return parseObject();
    if (c == 't' || c == 'f' || c == 'n') return parseLiteral();
    if (c == '-' || std::isdigit(static_cast<unsigned char>(c))) return parseNumber();
    throw std::runtime_error(std::string("unexpected character: ") + c);
}

JsonValue JsonParser::parseString() {
    consume(); // opening "
    std::string result;
    while (true) {
        char c = consume();
        if (c == '"') break;
        if (c == '\\') {
            char esc = consume();
            switch (esc) {
                case '"':  result += '"';  break;
                case '\\': result += '\\'; break;
                case 'n':  result += '\n'; break;
                case 't':  result += '\t'; break;
                case 'r':  result += '\r'; break;
                default:   result += esc;
            }
        } else {
            result += c;
        }
    }
    return JsonValue(result);
}

JsonValue JsonParser::parseNumber() {
    size_t start = pos_;
    bool isDouble = false;
    if (peek() == '-') consume();
    while (std::isdigit(static_cast<unsigned char>(peek()))) consume();
    if (peek() == '.') { isDouble = true; consume(); while (std::isdigit(static_cast<unsigned char>(peek()))) consume(); }
    if (peek() == 'e' || peek() == 'E') { isDouble = true; consume(); if (peek() == '+' || peek() == '-') consume(); while (std::isdigit(static_cast<unsigned char>(peek()))) consume(); }
    std::string token = input_.substr(start, pos_ - start);
    if (isDouble) return JsonValue(std::stod(token));
    return JsonValue(std::stoi(token));
}

JsonValue JsonParser::parseLiteral() {
    if (input_.substr(pos_, 4) == "true")  { pos_ += 4; return JsonValue(true);  }
    if (input_.substr(pos_, 5) == "false") { pos_ += 5; return JsonValue(false); }
    if (input_.substr(pos_, 4) == "null")  { pos_ += 4; return JsonValue();      }
    throw std::runtime_error("invalid literal");
}

JsonValue JsonParser::parseArray() {
    consume(); // '['
    std::vector<JsonValue> arr;
    skipWhitespace();
    if (peek() == ']') { consume(); return JsonValue(std::move(arr)); }
    while (true) {
        skipWhitespace();
        arr.push_back(parseValue());
        skipWhitespace();
        if (peek() == ']') { consume(); break; }
        if (peek() == ',') { consume(); continue; }
        throw std::runtime_error("expected ',' or ']' in array");
    }
    return JsonValue(std::move(arr));
}

JsonValue JsonParser::parseObject() {
    consume(); // '{'
    std::map<std::string, JsonValue> obj;
    skipWhitespace();
    if (peek() == '}') { consume(); return JsonValue(std::move(obj)); }
    while (true) {
        skipWhitespace();
        if (peek() != '"') throw std::runtime_error("expected string key");
        JsonValue keyVal = parseString();
        skipWhitespace();
        if (consume() != ':') throw std::runtime_error("expected ':'");
        skipWhitespace();
        JsonValue val = parseValue();
        obj[keyVal.asString()] = std::move(val);
        skipWhitespace();
        if (peek() == '}') { consume(); break; }
        if (peek() == ',') { consume(); continue; }
        throw std::runtime_error("expected ',' or '}' in object");
    }
    return JsonValue(std::move(obj));
}
