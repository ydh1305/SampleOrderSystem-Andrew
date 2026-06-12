#include <gmock/gmock.h>
#include "model/OrderStatus.h"
#include "model/JobStatus.h"

// Cycle 1: Enum string conversion (T1~T3)

// T1: toString - all 5 OrderStatus values
TEST(OrderStatusTest, ToStringAllValues) {
    EXPECT_EQ(toString(OrderStatus::RESERVED),  "RESERVED");
    EXPECT_EQ(toString(OrderStatus::REJECTED),  "REJECTED");
    EXPECT_EQ(toString(OrderStatus::PRODUCING), "PRODUCING");
    EXPECT_EQ(toString(OrderStatus::CONFIRMED), "CONFIRMED");
    EXPECT_EQ(toString(OrderStatus::RELEASED),  "RELEASED");
}

// T2: fromString - all 5 OrderStatus values
TEST(OrderStatusTest, FromStringAllValues) {
    EXPECT_EQ(orderStatusFromString("RESERVED"),  OrderStatus::RESERVED);
    EXPECT_EQ(orderStatusFromString("REJECTED"),  OrderStatus::REJECTED);
    EXPECT_EQ(orderStatusFromString("PRODUCING"), OrderStatus::PRODUCING);
    EXPECT_EQ(orderStatusFromString("CONFIRMED"), OrderStatus::CONFIRMED);
    EXPECT_EQ(orderStatusFromString("RELEASED"),  OrderStatus::RELEASED);
}

// T3: toString -> fromString round-trip
TEST(OrderStatusTest, RoundTrip) {
    OrderStatus values[] = {
        OrderStatus::RESERVED, OrderStatus::REJECTED,
        OrderStatus::PRODUCING, OrderStatus::CONFIRMED, OrderStatus::RELEASED
    };
    for (auto s : values) {
        EXPECT_EQ(orderStatusFromString(toString(s)), s);
    }
}
