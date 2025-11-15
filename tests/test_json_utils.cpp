#include <gtest/gtest.h>
#include "../src/utils/json_utils.hpp"

using namespace vzcode::utils;

class JsonUtilsTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }
};

// Test JSON Value Creation
TEST_F(JsonUtilsTest, CreateNullValue) {
    JsonValue val;
    EXPECT_EQ(val.type(), JsonValue::Type::Null);
}

TEST_F(JsonUtilsTest, CreateBooleanValue) {
    JsonValue val_true(true);
    JsonValue val_false(false);

    EXPECT_TRUE(val_true.as_bool());
    EXPECT_FALSE(val_false.as_bool());
}

TEST_F(JsonUtilsTest, CreateNumberValue) {
    JsonValue val_int(42);
    JsonValue val_double(3.14);

    EXPECT_DOUBLE_EQ(val_int.as_number(), 42.0);
    EXPECT_DOUBLE_EQ(val_double.as_number(), 3.14);
}

TEST_F(JsonUtilsTest, CreateStringValue) {
    JsonValue val("hello");
    EXPECT_EQ(val.as_string(), "hello");

    JsonValue val2(std::string("world"));
    EXPECT_EQ(val2.as_string(), "world");
}

TEST_F(JsonUtilsTest, CreateArrayValue) {
    JsonValue arr = JsonValue::array();
    EXPECT_EQ(arr.array_size(), 0);

    arr.push_back(JsonValue(1));
    arr.push_back(JsonValue(2));
    arr.push_back(JsonValue(3));

    EXPECT_EQ(arr.array_size(), 3);
    EXPECT_DOUBLE_EQ(arr[0].as_number(), 1.0);
    EXPECT_DOUBLE_EQ(arr[1].as_number(), 2.0);
    EXPECT_DOUBLE_EQ(arr[2].as_number(), 3.0);
}

TEST_F(JsonUtilsTest, CreateObjectValue) {
    JsonValue obj = JsonValue::object();

    obj["name"] = JsonValue("John");
    obj["age"] = JsonValue(30);
    obj["active"] = JsonValue(true);

    EXPECT_TRUE(obj.has_key("name"));
    EXPECT_TRUE(obj.has_key("age"));
    EXPECT_TRUE(obj.has_key("active"));
    EXPECT_FALSE(obj.has_key("missing"));

    EXPECT_EQ(obj["name"].as_string(), "John");
    EXPECT_DOUBLE_EQ(obj["age"].as_number(), 30.0);
    EXPECT_TRUE(obj["active"].as_bool());
}

// Test JSON Stringification
TEST_F(JsonUtilsTest, StringifyNull) {
    JsonValue val;
    EXPECT_EQ(val.stringify(), "null");
}

TEST_F(JsonUtilsTest, StringifyBoolean) {
    EXPECT_EQ(JsonValue(true).stringify(), "true");
    EXPECT_EQ(JsonValue(false).stringify(), "false");
}

TEST_F(JsonUtilsTest, StringifyNumber) {
    EXPECT_EQ(JsonValue(42).stringify(), "42");
    EXPECT_EQ(JsonValue(3.14).stringify(), "3.14");
}

TEST_F(JsonUtilsTest, StringifyString) {
    EXPECT_EQ(JsonValue("hello").stringify(), "\"hello\"");
    EXPECT_EQ(JsonValue("").stringify(), "\"\"");
}

TEST_F(JsonUtilsTest, StringifyStringWithEscapes) {
    EXPECT_EQ(JsonValue("hello\"world").stringify(), "\"hello\\\"world\"");
    EXPECT_EQ(JsonValue("line1\nline2").stringify(), "\"line1\\nline2\"");
    EXPECT_EQ(JsonValue("tab\there").stringify(), "\"tab\\there\"");
}

TEST_F(JsonUtilsTest, StringifyArray) {
    JsonValue arr = JsonValue::array();
    arr.push_back(JsonValue(1));
    arr.push_back(JsonValue(2));
    arr.push_back(JsonValue(3));

    EXPECT_EQ(arr.stringify(), "[1,2,3]");
}

TEST_F(JsonUtilsTest, StringifyEmptyArray) {
    JsonValue arr = JsonValue::array();
    EXPECT_EQ(arr.stringify(), "[]");
}

TEST_F(JsonUtilsTest, StringifyObject) {
    JsonValue obj = JsonValue::object();
    obj["name"] = JsonValue("John");
    obj["age"] = JsonValue(30);

    std::string result = obj.stringify();
    // Object order may vary, so just check it contains the right parts
    EXPECT_NE(result.find("\"name\":\"John\""), std::string::npos);
    EXPECT_NE(result.find("\"age\":30"), std::string::npos);
}

TEST_F(JsonUtilsTest, StringifyNestedStructure) {
    JsonValue obj = JsonValue::object();
    obj["name"] = JsonValue("John");

    JsonValue arr = JsonValue::array();
    arr.push_back(JsonValue(1));
    arr.push_back(JsonValue(2));
    obj["numbers"] = arr;

    std::string result = obj.stringify();
    EXPECT_NE(result.find("\"name\":\"John\""), std::string::npos);
    EXPECT_NE(result.find("\"numbers\":[1,2]"), std::string::npos);
}

// Test JSON Parsing
TEST_F(JsonUtilsTest, ParseNull) {
    JsonValue val = JsonValue::parse("null");
    EXPECT_EQ(val.type(), JsonValue::Type::Null);
}

TEST_F(JsonUtilsTest, ParseBoolean) {
    JsonValue val_true = JsonValue::parse("true");
    JsonValue val_false = JsonValue::parse("false");

    EXPECT_TRUE(val_true.as_bool());
    EXPECT_FALSE(val_false.as_bool());
}

TEST_F(JsonUtilsTest, ParseNumber) {
    JsonValue val1 = JsonValue::parse("42");
    JsonValue val2 = JsonValue::parse("3.14");
    JsonValue val3 = JsonValue::parse("-10");

    EXPECT_DOUBLE_EQ(val1.as_number(), 42.0);
    EXPECT_DOUBLE_EQ(val2.as_number(), 3.14);
    EXPECT_DOUBLE_EQ(val3.as_number(), -10.0);
}

TEST_F(JsonUtilsTest, ParseString) {
    JsonValue val = JsonValue::parse("\"hello world\"");
    EXPECT_EQ(val.as_string(), "hello world");
}

TEST_F(JsonUtilsTest, ParseStringWithEscapes) {
    JsonValue val1 = JsonValue::parse("\"hello\\\"world\"");
    EXPECT_EQ(val1.as_string(), "hello\"world");

    JsonValue val2 = JsonValue::parse("\"line1\\nline2\"");
    EXPECT_EQ(val2.as_string(), "line1\nline2");
}

TEST_F(JsonUtilsTest, ParseArray) {
    JsonValue arr = JsonValue::parse("[1, 2, 3]");

    EXPECT_EQ(arr.array_size(), 3);
    EXPECT_DOUBLE_EQ(arr[0].as_number(), 1.0);
    EXPECT_DOUBLE_EQ(arr[1].as_number(), 2.0);
    EXPECT_DOUBLE_EQ(arr[2].as_number(), 3.0);
}

TEST_F(JsonUtilsTest, ParseEmptyArray) {
    JsonValue arr = JsonValue::parse("[]");
    EXPECT_EQ(arr.array_size(), 0);
}

TEST_F(JsonUtilsTest, ParseObject) {
    JsonValue obj = JsonValue::parse("{\"name\": \"John\", \"age\": 30}");

    EXPECT_TRUE(obj.has_key("name"));
    EXPECT_TRUE(obj.has_key("age"));
    EXPECT_EQ(obj["name"].as_string(), "John");
    EXPECT_DOUBLE_EQ(obj["age"].as_number(), 30.0);
}

TEST_F(JsonUtilsTest, ParseEmptyObject) {
    JsonValue obj = JsonValue::parse("{}");
    EXPECT_EQ(obj.keys().size(), 0);
}

TEST_F(JsonUtilsTest, ParseNestedStructure) {
    std::string json = R"({
        "name": "John",
        "age": 30,
        "hobbies": ["reading", "coding"],
        "address": {
            "city": "New York",
            "zip": 10001
        }
    })";

    JsonValue obj = JsonValue::parse(json);

    EXPECT_EQ(obj["name"].as_string(), "John");
    EXPECT_DOUBLE_EQ(obj["age"].as_number(), 30.0);
    EXPECT_EQ(obj["hobbies"].array_size(), 2);
    EXPECT_EQ(obj["hobbies"][0].as_string(), "reading");
    EXPECT_EQ(obj["address"]["city"].as_string(), "New York");
    EXPECT_DOUBLE_EQ(obj["address"]["zip"].as_number(), 10001.0);
}

TEST_F(JsonUtilsTest, ParseWithWhitespace) {
    JsonValue obj = JsonValue::parse("  {  \"key\"  :  \"value\"  }  ");
    EXPECT_EQ(obj["key"].as_string(), "value");
}

// Test Round-trip (parse then stringify)
TEST_F(JsonUtilsTest, RoundTripSimple) {
    std::string original = "{\"name\":\"John\",\"age\":30}";
    JsonValue parsed = JsonValue::parse(original);
    std::string serialized = parsed.stringify();

    // Parse again to compare values
    JsonValue reparsed = JsonValue::parse(serialized);
    EXPECT_EQ(reparsed["name"].as_string(), "John");
    EXPECT_DOUBLE_EQ(reparsed["age"].as_number(), 30.0);
}

// Test Error Handling
TEST_F(JsonUtilsTest, ParseInvalidJson) {
    EXPECT_THROW(JsonValue::parse("{invalid}"), std::exception);
    EXPECT_THROW(JsonValue::parse("[1, 2,]"), std::exception);
    EXPECT_THROW(JsonValue::parse(""), std::exception);
}

TEST_F(JsonUtilsTest, TypeMismatchThrows) {
    JsonValue str("hello");
    EXPECT_THROW(str.as_number(), std::exception);
    EXPECT_THROW(str.as_bool(), std::exception);

    JsonValue num(42);
    EXPECT_THROW(num.as_string(), std::exception);
    EXPECT_THROW(num.as_bool(), std::exception);
}

TEST_F(JsonUtilsTest, ObjectKeys) {
    JsonValue obj = JsonValue::object();
    obj["a"] = JsonValue(1);
    obj["b"] = JsonValue(2);
    obj["c"] = JsonValue(3);

    std::vector<std::string> keys = obj.keys();
    EXPECT_EQ(keys.size(), 3);

    // Check all keys are present (order may vary)
    EXPECT_NE(std::find(keys.begin(), keys.end(), "a"), keys.end());
    EXPECT_NE(std::find(keys.begin(), keys.end(), "b"), keys.end());
    EXPECT_NE(std::find(keys.begin(), keys.end(), "c"), keys.end());
}

// Test edge cases
TEST_F(JsonUtilsTest, EmptyStringKey) {
    JsonValue obj = JsonValue::object();
    obj[""] = JsonValue("empty key");

    EXPECT_TRUE(obj.has_key(""));
    EXPECT_EQ(obj[""].as_string(), "empty key");
}

TEST_F(JsonUtilsTest, LargeNumbers) {
    JsonValue val = JsonValue::parse("999999999999.999");
    EXPECT_DOUBLE_EQ(val.as_number(), 999999999999.999);
}

TEST_F(JsonUtilsTest, ScientificNotation) {
    JsonValue val = JsonValue::parse("1.5e10");
    EXPECT_DOUBLE_EQ(val.as_number(), 1.5e10);
}
