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

// ── Cycle 2: 컬렉션 타입 파싱 (T7~T11) ────────────────────────────────

TEST(JsonParserTest, ParseArray) {
    auto val = JsonParser::parse("[1, 2, 3]");
    EXPECT_EQ(val.type(), JsonType::Array);
    EXPECT_EQ(val.asArray().size(), 3u);
    EXPECT_EQ(val.asArray()[0].asInt(), 1);
    EXPECT_EQ(val.asArray()[2].asInt(), 3);
}

TEST(JsonParserTest, ParseObject) {
    auto val = JsonParser::parse("{\"key\": \"val\"}");
    EXPECT_EQ(val.type(), JsonType::Object);
    EXPECT_EQ(val["key"].asString(), "val");
}

TEST(JsonParserTest, ParseNestedObject) {
    auto val = JsonParser::parse("{\"a\": {\"b\": 1}}");
    EXPECT_EQ(val["a"]["b"].asInt(), 1);
}

TEST(JsonParserTest, ParseWithWhitespace) {
    auto val = JsonParser::parse("  {  \"k\"  :  1  }  ");
    EXPECT_EQ(val["k"].asInt(), 1);
}

TEST(JsonParserTest, ParseEmptyArray) {
    auto val = JsonParser::parse("[]");
    EXPECT_EQ(val.type(), JsonType::Array);
    EXPECT_TRUE(val.asArray().empty());
}

TEST(JsonParserTest, ParseEmptyObject) {
    auto val = JsonParser::parse("{}");
    EXPECT_EQ(val.type(), JsonType::Object);
    EXPECT_TRUE(val.asObject().empty());
}
