#include <gmock/gmock.h>
#include "model/OrderIdGenerator.h"

// T1: generate returns formatted order ID
TEST(OrderIdGeneratorTest, GenerateFormatsCorrectly) {
    EXPECT_EQ(OrderIdGenerator::generate("20260612", 1),    "ORD-20260612-0001");
    EXPECT_EQ(OrderIdGenerator::generate("20260612", 42),   "ORD-20260612-0042");
    EXPECT_EQ(OrderIdGenerator::generate("20260612", 9999), "ORD-20260612-9999");
}

// T2: generate pads sequence to 4 digits
TEST(OrderIdGeneratorTest, GeneratePadsSequenceTo4Digits) {
    std::string id = OrderIdGenerator::generate("20260612", 1);
    EXPECT_EQ(id, "ORD-20260612-0001");
    EXPECT_EQ(id.size(), 19u);
}

// T3: getDatePart extracts date from order ID
TEST(OrderIdGeneratorTest, GetDatePartExtractsDate) {
    EXPECT_EQ(OrderIdGenerator::getDatePart("ORD-20260612-0001"), "20260612");
}

// T4: getSequencePart extracts sequence number
TEST(OrderIdGeneratorTest, GetSequencePartExtractsSequence) {
    EXPECT_EQ(OrderIdGenerator::getSequencePart("ORD-20260612-0042"), 42);
    EXPECT_EQ(OrderIdGenerator::getSequencePart("ORD-20260612-0001"), 1);
}

// T5: isValidOrderId returns true for valid IDs
TEST(OrderIdGeneratorTest, IsValidOrderIdReturnsTrueForValid) {
    EXPECT_TRUE(OrderIdGenerator::isValidOrderId("ORD-20260612-0001"));
    EXPECT_FALSE(OrderIdGenerator::isValidOrderId("INVALID"));
    EXPECT_FALSE(OrderIdGenerator::isValidOrderId("ORD-2026-0001"));
}
