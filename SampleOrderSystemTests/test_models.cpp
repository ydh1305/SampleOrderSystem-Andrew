#include <gmock/gmock.h>
#include "model/OrderStatus.h"
#include "model/JobStatus.h"
#include "model/Sample.h"
#include "model/Order.h"
#include "model/ProductionJob.h"

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

// Cycle 2: Sample serialization (T4~T6)

// T4: toJson contains all fields
TEST(SampleModelTest, ToJsonContainsAllFields) {
    Sample s;
    s.sampleId          = "S-001";
    s.name              = "Silicon Wafer";
    s.avgProductionTime = 0.8;
    s.yieldRate         = 0.92;
    s.stock             = 480;

    JsonValue json = s.toJson();
    EXPECT_EQ(json["sampleId"].asString(),          "S-001");
    EXPECT_EQ(json["name"].asString(),              "Silicon Wafer");
    EXPECT_DOUBLE_EQ(json["avgProductionTime"].asDouble(), 0.8);
    EXPECT_DOUBLE_EQ(json["yieldRate"].asDouble(),  0.92);
    EXPECT_EQ(json["stock"].asInt(),                480);
}

// T5: fromJson restores all fields
TEST(SampleModelTest, FromJsonRestoresAllFields) {
    std::map<std::string, JsonValue> obj;
    obj["sampleId"]          = JsonValue(std::string("S-002"));
    obj["name"]              = JsonValue(std::string("GaAs Wafer"));
    obj["avgProductionTime"] = JsonValue(1.2);
    obj["yieldRate"]         = JsonValue(0.85);
    obj["stock"]             = JsonValue(200);

    Sample s = Sample::fromJson(JsonValue(std::move(obj)));
    EXPECT_EQ(s.sampleId,           "S-002");
    EXPECT_EQ(s.name,               "GaAs Wafer");
    EXPECT_DOUBLE_EQ(s.avgProductionTime, 1.2);
    EXPECT_DOUBLE_EQ(s.yieldRate,   0.85);
    EXPECT_EQ(s.stock,              200);
}

// T6: toJson -> fromJson round-trip
TEST(SampleModelTest, RoundTrip) {
    Sample original;
    original.sampleId          = "S-003";
    original.name              = "InP Wafer";
    original.avgProductionTime = 0.5;
    original.yieldRate         = 0.95;
    original.stock             = 100;

    Sample restored = Sample::fromJson(original.toJson());
    EXPECT_EQ(restored.sampleId,           original.sampleId);
    EXPECT_EQ(restored.name,               original.name);
    EXPECT_DOUBLE_EQ(restored.avgProductionTime, original.avgProductionTime);
    EXPECT_DOUBLE_EQ(restored.yieldRate,   original.yieldRate);
    EXPECT_EQ(restored.stock,              original.stock);
}

// Cycle 3: Order serialization (T7~T9)

// T7: status field serialized as string
TEST(OrderModelTest, StatusFieldSerializedAsString) {
    Order o;
    o.status = OrderStatus::RESERVED;
    EXPECT_EQ(o.toJson()["status"].asString(), "RESERVED");
}

// T8: fromJson restores status as enum
TEST(OrderModelTest, FromJsonRestoresStatus) {
    std::map<std::string, JsonValue> obj;
    obj["orderId"]      = JsonValue(std::string("ORD-20260612-0001"));
    obj["sampleId"]     = JsonValue(std::string("S-001"));
    obj["customerName"] = JsonValue(std::string("SK Hynix"));
    obj["quantity"]     = JsonValue(150);
    obj["status"]       = JsonValue(std::string("CONFIRMED"));
    obj["createdAt"]    = JsonValue(std::string("2026-06-12 09:00:00"));
    obj["updatedAt"]    = JsonValue(std::string("2026-06-12 09:00:00"));

    Order o = Order::fromJson(JsonValue(std::move(obj)));
    EXPECT_EQ(o.status, OrderStatus::CONFIRMED);
    EXPECT_EQ(o.quantity, 150);
}

// T9: toJson -> fromJson round-trip
TEST(OrderModelTest, RoundTrip) {
    Order original;
    original.orderId      = "ORD-20260612-0001";
    original.sampleId     = "S-001";
    original.customerName = "SK Hynix";
    original.quantity     = 150;
    original.status       = OrderStatus::PRODUCING;
    original.createdAt    = "2026-06-12 09:00:00";
    original.updatedAt    = "2026-06-12 10:00:00";

    Order restored = Order::fromJson(original.toJson());
    EXPECT_EQ(restored.orderId,      original.orderId);
    EXPECT_EQ(restored.sampleId,     original.sampleId);
    EXPECT_EQ(restored.customerName, original.customerName);
    EXPECT_EQ(restored.quantity,     original.quantity);
    EXPECT_EQ(restored.status,       original.status);
    EXPECT_EQ(restored.createdAt,    original.createdAt);
    EXPECT_EQ(restored.updatedAt,    original.updatedAt);
}

// Cycle 4: ProductionJob serialization (T10~T11)

// T10: toJson includes empty string fields (startedAt, completedAt)
TEST(ProductionJobModelTest, ToJsonIncludesEmptyStringFields) {
    ProductionJob job;
    job.jobId      = "JOB-20260612-0001";
    job.startedAt  = "";
    job.completedAt = "";

    JsonValue json = job.toJson();
    EXPECT_EQ(json["startedAt"].asString(),   "");
    EXPECT_EQ(json["completedAt"].asString(), "");
}

// T11: toJson -> fromJson round-trip
TEST(ProductionJobModelTest, RoundTrip) {
    ProductionJob original;
    original.jobId       = "JOB-20260612-0001";
    original.orderId     = "ORD-20260612-0043";
    original.sampleId    = "S-003";
    original.shortage    = 170;
    original.actualProd  = 206;
    original.totalTime   = 164.8;
    original.status      = JobStatus::WAITING;
    original.enqueuedAt  = "2026-06-12 09:15:00";
    original.startedAt   = "";
    original.completedAt = "";

    ProductionJob restored = ProductionJob::fromJson(original.toJson());
    EXPECT_EQ(restored.jobId,       original.jobId);
    EXPECT_EQ(restored.orderId,     original.orderId);
    EXPECT_EQ(restored.sampleId,    original.sampleId);
    EXPECT_EQ(restored.shortage,    original.shortage);
    EXPECT_EQ(restored.actualProd,  original.actualProd);
    EXPECT_DOUBLE_EQ(restored.totalTime, original.totalTime);
    EXPECT_EQ(restored.status,      original.status);
    EXPECT_EQ(restored.enqueuedAt,  original.enqueuedAt);
    EXPECT_EQ(restored.startedAt,   original.startedAt);
    EXPECT_EQ(restored.completedAt, original.completedAt);
}
