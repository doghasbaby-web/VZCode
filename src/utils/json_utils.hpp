#ifndef JSON_UTILS_HPP
#define JSON_UTILS_HPP

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace vzcode {
namespace utils {

/**
 * Lightweight JSON utility class for parsing and building JSON
 * Avoids external dependencies for WASM compatibility
 */
class JsonValue {
public:
    enum class Type {
        Null,
        Boolean,
        Number,
        String,
        Array,
        Object
    };

    JsonValue();
    explicit JsonValue(bool value);
    explicit JsonValue(int value);
    explicit JsonValue(double value);
    explicit JsonValue(const std::string& value);
    explicit JsonValue(const char* value);

    static JsonValue parse(const std::string& json_str);
    std::string stringify() const;

    // Type checking
    Type type() const { return type_; }
    bool is_null() const { return type_ == Type::Null; }
    bool is_bool() const { return type_ == Type::Boolean; }
    bool is_number() const { return type_ == Type::Number; }
    bool is_string() const { return type_ == Type::String; }
    bool is_array() const { return type_ == Type::Array; }
    bool is_object() const { return type_ == Type::Object; }

    // Value getters
    bool as_bool() const;
    double as_number() const;
    std::string as_string() const;

    // Array operations
    size_t array_size() const;
    JsonValue& operator[](size_t index);
    const JsonValue& operator[](size_t index) const;
    void push_back(const JsonValue& value);

    // Object operations
    JsonValue& operator[](const std::string& key);
    const JsonValue& operator[](const std::string& key) const;
    bool has_key(const std::string& key) const;
    std::vector<std::string> keys() const;

    // Factory methods
    static JsonValue array();
    static JsonValue object();

private:
    Type type_;
    bool bool_value_;
    double number_value_;
    std::string string_value_;
    std::vector<JsonValue> array_value_;
    std::map<std::string, JsonValue> object_value_;

    static JsonValue parse_value(const std::string& str, size_t& pos);
    static JsonValue parse_object(const std::string& str, size_t& pos);
    static JsonValue parse_array(const std::string& str, size_t& pos);
    static JsonValue parse_string(const std::string& str, size_t& pos);
    static JsonValue parse_number(const std::string& str, size_t& pos);
    static void skip_whitespace(const std::string& str, size_t& pos);
};

} // namespace utils
} // namespace vzcode

#endif // JSON_UTILS_HPP
