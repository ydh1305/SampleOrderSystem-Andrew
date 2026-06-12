#pragma once
#include <string>

class OrderIdGenerator {
public:
    static std::string generate(const std::string& date, int sequence);
    static std::string getDatePart(const std::string& orderId);
    static int         getSequencePart(const std::string& orderId);
    static bool        isValidOrderId(const std::string& orderId);
};
