#include <gmock/gmock.h>
#include "storage/JsonParser.h"

// ── Cycle 1: 스칼라 타입 파싱 (T1~T5) ─────────────────────────────────

TEST(JsonParserTest, ParseString) {
    auto val = JsonParser::parse("\"hello\"");
    EXPECT_EQ(val.type(), JsonType::String);
    EXPECT_EQ(val.asString(), "hello");
}

TEST(JsonParserTest, ParseInt) {
    auto val = JsonParser::parse("42");
    EXPECT_EQ(val.type(), JsonType::Int);
    EXPECT_EQ(val.asInt(), 42);
}

TEST(JsonParserTest, ParseDouble) {
    auto val = JsonParser::parse("3.14");
    EXPECT_EQ(val.type(), JsonType::Double);
    EXPECT_DOUBLE_EQ(val.asDouble(), 3.14);
}

TEST(JsonParserTest, ParseBoolTrue) {
    auto val = JsonParser::parse("true");
    EXPECT_EQ(val.type(), JsonType::Bool);
    EXPECT_TRUE(val.asBool());
}

TEST(JsonParserTest, ParseBoolFalse) {
    auto val = JsonParser::parse("false");
    EXPECT_EQ(val.type(), JsonType::Bool);
    EXPECT_FALSE(val.asBool());
}

TEST(JsonParserTest, ParseNull) {
    auto val = JsonParser::parse("null");
    EXPECT_EQ(val.type(), JsonType::Null);
}
