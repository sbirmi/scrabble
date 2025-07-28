#include <gtest/gtest.h>
#include "json_util.hpp"

class JsonUtilTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(JsonUtilTest, JsonifyInt) {
    Json::Value result = jsonify(42);
    EXPECT_TRUE(result.isInt());
    EXPECT_EQ(result.asInt(), 42);
    
    result = jsonify(0);
    EXPECT_EQ(result.asInt(), 0);
    
    result = jsonify(-100);
    EXPECT_EQ(result.asInt(), -100);
}

TEST_F(JsonUtilTest, JsonifyString) {
    Json::Value result = jsonify(std::string("hello"));
    EXPECT_TRUE(result.isArray());
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result[0].asString(), "hello");
    
    result = jsonify(std::string(""));
    EXPECT_TRUE(result.isArray());
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result[0].asString(), "");
    
    result = jsonify(std::string("test with spaces"));
    EXPECT_TRUE(result.isArray());
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result[0].asString(), "test with spaces");
}

TEST_F(JsonUtilTest, JsonifyTwoStrings) {
    Json::Value result = jsonify(std::string("first"), std::string("second"));
    EXPECT_TRUE(result.isArray());
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result[0].asString(), "first");
    EXPECT_EQ(result[1].asString(), "second");
}

TEST_F(JsonUtilTest, JsonifyThreeStrings) {
    Json::Value result = jsonify(std::string("one"), std::string("two"), std::string("three"));
    EXPECT_TRUE(result.isArray());
    EXPECT_EQ(result.size(), 3);
    EXPECT_EQ(result[0].asString(), "one");
    EXPECT_EQ(result[1].asString(), "two");
    EXPECT_EQ(result[2].asString(), "three");
}

TEST_F(JsonUtilTest, JsonifyCharAndPosition) {
    Json::Value result = jsonify('A', 5u, 10u);
    EXPECT_TRUE(result.isArray());
    EXPECT_EQ(result.size(), 3);
    EXPECT_EQ(result[0].asString(), "A");
    EXPECT_EQ(result[1].asUInt(), 5);
    EXPECT_EQ(result[2].asUInt(), 10);
    
    result = jsonify('Z', 0u, 0u);
    EXPECT_EQ(result[0].asString(), "Z");
    EXPECT_EQ(result[1].asUInt(), 0);
    EXPECT_EQ(result[2].asUInt(), 0);
}

TEST_F(JsonUtilTest, JsonifyThreeStringsAndUint) {
    Json::Value result = jsonify(std::string("cmd"), std::string("param1"), std::string("param2"), 42u);
    EXPECT_TRUE(result.isArray());
    EXPECT_EQ(result.size(), 4);
    EXPECT_EQ(result[0].asString(), "cmd");
    EXPECT_EQ(result[1].asString(), "param1");
    EXPECT_EQ(result[2].asString(), "param2");
    EXPECT_EQ(result[3].asUInt(), 42);
}

TEST_F(JsonUtilTest, JsonifyMoveType) {
    // Test move types: 0-pass, 1-exch, 2-play
    Json::Value result = jsonify(0u, std::string("pass"), 0);
    EXPECT_TRUE(result.isArray());
    EXPECT_EQ(result.size(), 3);
    EXPECT_EQ(result[0].asUInt(), 0);
    EXPECT_EQ(result[1].asString(), "pass");
    EXPECT_EQ(result[2].asInt(), 0);
    
    result = jsonify(1u, std::string("ABC"), -5);
    EXPECT_EQ(result[0].asUInt(), 1);
    EXPECT_EQ(result[1].asString(), "ABC");
    EXPECT_EQ(result[2].asInt(), -5);
    
    result = jsonify(2u, std::string("HELLO"), 25);
    EXPECT_EQ(result[0].asUInt(), 2);
    EXPECT_EQ(result[1].asString(), "HELLO");
    EXPECT_EQ(result[2].asInt(), 25);
}

TEST_F(JsonUtilTest, JsonifySpecialCharacters) {
    Json::Value result = jsonify(std::string("\"quotes\""));
    EXPECT_TRUE(result.isArray());
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result[0].asString(), "\"quotes\"");
    
    result = jsonify(std::string("line\nbreak"));
    EXPECT_TRUE(result.isArray());
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result[0].asString(), "line\nbreak");
    
    result = jsonify('\t', 1u, 2u);
    EXPECT_TRUE(result.isArray());
    EXPECT_EQ(result.size(), 3);
    EXPECT_EQ(result[0].asString(), "\t");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}