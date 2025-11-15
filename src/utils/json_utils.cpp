#include "json_utils.hpp"
#include <sstream>
#include <stdexcept>
#include <cctype>
#include <cmath>

namespace vzcode {
namespace utils {

JsonValue::JsonValue() : type_(Type::Null), bool_value_(false), number_value_(0.0) {}

JsonValue::JsonValue(bool value) : type_(Type::Boolean), bool_value_(value), number_value_(0.0) {}

JsonValue::JsonValue(int value) : type_(Type::Number), bool_value_(false), number_value_(static_cast<double>(value)) {}

JsonValue::JsonValue(double value) : type_(Type::Number), bool_value_(false), number_value_(value) {}

JsonValue::JsonValue(const std::string& value) : type_(Type::String), bool_value_(false), number_value_(0.0), string_value_(value) {}

JsonValue::JsonValue(const char* value) : type_(Type::String), bool_value_(false), number_value_(0.0), string_value_(value) {}

JsonValue JsonValue::array() {
    JsonValue val;
    val.type_ = Type::Array;
    return val;
}

JsonValue JsonValue::object() {
    JsonValue val;
    val.type_ = Type::Object;
    return val;
}

bool JsonValue::as_bool() const {
    if (type_ != Type::Boolean) throw std::runtime_error("Not a boolean");
    return bool_value_;
}

double JsonValue::as_number() const {
    if (type_ != Type::Number) throw std::runtime_error("Not a number");
    return number_value_;
}

std::string JsonValue::as_string() const {
    if (type_ != Type::String) throw std::runtime_error("Not a string");
    return string_value_;
}

size_t JsonValue::array_size() const {
    if (type_ != Type::Array) throw std::runtime_error("Not an array");
    return array_value_.size();
}

JsonValue& JsonValue::operator[](size_t index) {
    if (type_ != Type::Array) throw std::runtime_error("Not an array");
    if (index >= array_value_.size()) {
        array_value_.resize(index + 1);
    }
    return array_value_[index];
}

const JsonValue& JsonValue::operator[](size_t index) const {
    if (type_ != Type::Array) throw std::runtime_error("Not an array");
    return array_value_[index];
}

void JsonValue::push_back(const JsonValue& value) {
    if (type_ != Type::Array) throw std::runtime_error("Not an array");
    array_value_.push_back(value);
}

JsonValue& JsonValue::operator[](const std::string& key) {
    if (type_ != Type::Object) throw std::runtime_error("Not an object");
    return object_value_[key];
}

const JsonValue& JsonValue::operator[](const std::string& key) const {
    if (type_ != Type::Object) throw std::runtime_error("Not an object");
    auto it = object_value_.find(key);
    if (it == object_value_.end()) {
        static JsonValue null_value;
        return null_value;
    }
    return it->second;
}

bool JsonValue::has_key(const std::string& key) const {
    if (type_ != Type::Object) return false;
    return object_value_.find(key) != object_value_.end();
}

std::vector<std::string> JsonValue::keys() const {
    std::vector<std::string> result;
    if (type_ == Type::Object) {
        for (const auto& pair : object_value_) {
            result.push_back(pair.first);
        }
    }
    return result;
}

std::string JsonValue::stringify() const {
    std::stringstream ss;

    switch (type_) {
        case Type::Null:
            ss << "null";
            break;
        case Type::Boolean:
            ss << (bool_value_ ? "true" : "false");
            break;
        case Type::Number:
            if (std::floor(number_value_) == number_value_) {
                ss << static_cast<long long>(number_value_);
            } else {
                ss << number_value_;
            }
            break;
        case Type::String:
            ss << "\"";
            for (char c : string_value_) {
                if (c == '"') ss << "\\\"";
                else if (c == '\\') ss << "\\\\";
                else if (c == '\n') ss << "\\n";
                else if (c == '\r') ss << "\\r";
                else if (c == '\t') ss << "\\t";
                else ss << c;
            }
            ss << "\"";
            break;
        case Type::Array:
            ss << "[";
            for (size_t i = 0; i < array_value_.size(); ++i) {
                if (i > 0) ss << ",";
                ss << array_value_[i].stringify();
            }
            ss << "]";
            break;
        case Type::Object:
            ss << "{";
            bool first = true;
            for (const auto& pair : object_value_) {
                if (!first) ss << ",";
                first = false;
                ss << "\"" << pair.first << "\":" << pair.second.stringify();
            }
            ss << "}";
            break;
    }

    return ss.str();
}

void JsonValue::skip_whitespace(const std::string& str, size_t& pos) {
    while (pos < str.length() && std::isspace(str[pos])) {
        ++pos;
    }
}

JsonValue JsonValue::parse(const std::string& json_str) {
    size_t pos = 0;
    return parse_value(json_str, pos);
}

JsonValue JsonValue::parse_value(const std::string& str, size_t& pos) {
    skip_whitespace(str, pos);

    if (pos >= str.length()) {
        throw std::runtime_error("Unexpected end of JSON");
    }

    char c = str[pos];

    if (c == '{') {
        return parse_object(str, pos);
    } else if (c == '[') {
        return parse_array(str, pos);
    } else if (c == '"') {
        return parse_string(str, pos);
    } else if (c == 't' || c == 'f') {
        // Boolean
        if (str.substr(pos, 4) == "true") {
            pos += 4;
            return JsonValue(true);
        } else if (str.substr(pos, 5) == "false") {
            pos += 5;
            return JsonValue(false);
        }
    } else if (c == 'n') {
        // Null
        if (str.substr(pos, 4) == "null") {
            pos += 4;
            return JsonValue();
        }
    } else if (c == '-' || std::isdigit(c)) {
        return parse_number(str, pos);
    }

    throw std::runtime_error("Invalid JSON value");
}

JsonValue JsonValue::parse_object(const std::string& str, size_t& pos) {
    JsonValue obj = JsonValue::object();

    ++pos; // Skip '{'
    skip_whitespace(str, pos);

    if (str[pos] == '}') {
        ++pos;
        return obj;
    }

    while (true) {
        skip_whitespace(str, pos);

        // Parse key
        if (str[pos] != '"') throw std::runtime_error("Expected string key");
        JsonValue key = parse_string(str, pos);

        skip_whitespace(str, pos);
        if (str[pos] != ':') throw std::runtime_error("Expected ':'");
        ++pos;

        // Parse value
        JsonValue value = parse_value(str, pos);
        obj[key.as_string()] = value;

        skip_whitespace(str, pos);
        if (str[pos] == '}') {
            ++pos;
            break;
        } else if (str[pos] == ',') {
            ++pos;
        } else {
            throw std::runtime_error("Expected ',' or '}'");
        }
    }

    return obj;
}

JsonValue JsonValue::parse_array(const std::string& str, size_t& pos) {
    JsonValue arr = JsonValue::array();

    ++pos; // Skip '['
    skip_whitespace(str, pos);

    if (str[pos] == ']') {
        ++pos;
        return arr;
    }

    while (true) {
        JsonValue value = parse_value(str, pos);
        arr.push_back(value);

        skip_whitespace(str, pos);
        if (str[pos] == ']') {
            ++pos;
            break;
        } else if (str[pos] == ',') {
            ++pos;
        } else {
            throw std::runtime_error("Expected ',' or ']'");
        }
    }

    return arr;
}

JsonValue JsonValue::parse_string(const std::string& str, size_t& pos) {
    ++pos; // Skip opening '"'
    std::string result;

    while (pos < str.length() && str[pos] != '"') {
        if (str[pos] == '\\') {
            ++pos;
            if (pos >= str.length()) throw std::runtime_error("Unexpected end in string");

            switch (str[pos]) {
                case '"': result += '"'; break;
                case '\\': result += '\\'; break;
                case '/': result += '/'; break;
                case 'n': result += '\n'; break;
                case 'r': result += '\r'; break;
                case 't': result += '\t'; break;
                default: result += str[pos];
            }
        } else {
            result += str[pos];
        }
        ++pos;
    }

    if (pos >= str.length()) throw std::runtime_error("Unterminated string");
    ++pos; // Skip closing '"'

    return JsonValue(result);
}

JsonValue JsonValue::parse_number(const std::string& str, size_t& pos) {
    size_t start = pos;
    bool has_decimal = false;
    bool has_exponent = false;

    // Optional negative sign
    if (str[pos] == '-') ++pos;

    // Integer part (at least one digit required before decimal or exponent)
    if (pos >= str.length() || !std::isdigit(str[pos])) {
        throw std::runtime_error("Invalid number: expected digit");
    }

    // Parse integer digits
    while (pos < str.length() && std::isdigit(str[pos])) {
        ++pos;
    }

    // Optional decimal part
    if (pos < str.length() && str[pos] == '.') {
        has_decimal = true;
        ++pos;

        // At least one digit required after decimal point
        if (pos >= str.length() || !std::isdigit(str[pos])) {
            throw std::runtime_error("Invalid number: expected digit after decimal point");
        }

        while (pos < str.length() && std::isdigit(str[pos])) {
            ++pos;
        }
    }

    // Optional exponent part
    if (pos < str.length() && (str[pos] == 'e' || str[pos] == 'E')) {
        has_exponent = true;
        ++pos;

        // Optional sign in exponent
        if (pos < str.length() && (str[pos] == '+' || str[pos] == '-')) {
            ++pos;
        }

        // At least one digit required in exponent
        if (pos >= str.length() || !std::isdigit(str[pos])) {
            throw std::runtime_error("Invalid number: expected digit in exponent");
        }

        while (pos < str.length() && std::isdigit(str[pos])) {
            ++pos;
        }
    }

    std::string num_str = str.substr(start, pos - start);

    try {
        double value = std::stod(num_str);
        return JsonValue(value);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Invalid number format: ") + e.what());
    }
}

} // namespace utils
} // namespace vzcode
