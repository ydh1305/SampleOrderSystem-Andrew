#include <gmock/gmock.h>
#include "model/OrderStatus.h"
#include "model/JobStatus.h"

// ── Cycle 1: Enum 문자열 변환 (T1~T3) ─────────────────────────────────

// T1: toString - OrderStatus 전체 5개 값
TEST(OrderStatusTest, ToStringAllValues) {
    EXPECT_EQ(toString(OrderStatus::RESERVED),  "RESERVED");
    EXPECT_EQ(toString(OrderStatus::REJECTED),  "REJECTED");
    EXPECT_EQ(toString(OrderStatus::PRODUCING), "PRODUCING");
    EXPECT_EQ(toString(OrderStatus::CONFIRMED), "CONFIRMED");
    EXPECT_EQ(toString(OrderStatus::RELEASED),  "RELEASED");
}

// T2: fromString - OrderStatus 전체 5개 값
TEST(OrderStatusTest, FromStringAllValues) {
    EXPECT_EQ(orderStatusFromString("RESERVED"),  OrderStatus::RESERVED);
    EXPECT_EQ(orderStatusFromString("REJECTED"),  OrderStatus::REJECTED);
    EXPECT_EQ(orderStatusFromString("PRODUCING"), OrderStatus::PRODUCING);
    EXPECT_EQ(orderStatusFromString("CONFIRMED"), OrderStatus::CONFIRMED);
    EXPECT_EQ(orderStatusFromString("RELEASED"),  OrderStatus::RELEASED);
}

// T3: toString -> fromString 라운드트립
TEST(OrderStatusTest, RoundTrip) {
    for (auto s : {OrderStatus::RESERVED, OrderStatus::REJECTED,
                   OrderStatus::PRODUCING, OrderStatus::CONFIRMED,
                   OrderStatus::RELEASED}) {
        EXPECT_EQ(orderStatusFromString(toString(s)), s);
    }
}
