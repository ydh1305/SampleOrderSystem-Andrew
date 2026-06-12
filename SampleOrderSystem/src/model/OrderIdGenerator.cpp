#include "OrderIdGenerator.h"
#include <sstream>
#include <iomanip>

std::string OrderIdGenerator::generate(const std::string& date, int sequence) {
    std::ostringstream ss;
    ss << "ORD-" << date << "-" << std::setw(4) << std::setfill('0') << sequence;
    return ss.str();
}

std::string OrderIdGenerator::getDatePart(const std::string& orderId) {
    if (orderId.size() < 12) return "";
    return orderId.substr(4, 8);
}

int OrderIdGenerator::getSequencePart(const std::string& orderId) {
    if (orderId.size() < 17) return 0;
    try { return std::stoi(orderId.substr(13)); }
    catch (...) { return 0; }
}

bool OrderIdGenerator::isValidOrderId(const std::string& orderId) {
    // "ORD-YYYYMMDD-NNNN" = 17 chars
    if (orderId.size() != 17) return false;
    if (orderId.substr(0, 4) != "ORD-") return false;
    if (orderId[12] != '-') return false;
    for (int i = 4; i < 12; ++i)
        if (!std::isdigit(static_cast<unsigned char>(orderId[i]))) return false;
    for (int i = 13; i < 17; ++i)
        if (!std::isdigit(static_cast<unsigned char>(orderId[i]))) return false;
    return true;
}
