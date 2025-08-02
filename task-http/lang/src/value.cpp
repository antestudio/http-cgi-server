#include "value.h"
#include <cstdint>
#include <stdexcept>

Value::Value() : type(Type::VOID), data(0) {}

Value::Value(intmax_t val) : type(Type::INT), data(val) {}

Value::Value(double val) : type(Type::REAL), data(val) {}

Value::Value(const std::string& val) : type(Type::STRING), data(val) {}

Value::Value(bool val) : type(Type::BOOLEAN), data(val) {}

Type Value::getType() const { return type; }

intmax_t Value::getInt() const {
    if (type != Type::INT) throw std::runtime_error("Value is not an integer");
    return std::get<intmax_t>(data);
}

double Value::getReal() const {
    if (type != Type::REAL) throw std::runtime_error("Value is not a real number");
    return std::get<double>(data);
}

const std::string& Value::getString() const {
    if (type != Type::STRING) throw std::runtime_error("Value is not a string");
    return std::get<std::string>(data);
}

bool Value::getBoolean() const {
    if (type != Type::BOOLEAN) throw std::runtime_error("Value is not a boolean");
    return std::get<bool>(data);
}

std::string Value::toString() const {
    switch (type) {
        case Type::INT: return std::to_string(std::get<intmax_t>(data));
        case Type::REAL: return std::to_string(std::get<double>(data));
        case Type::STRING: return std::get<std::string>(data);
        case Type::BOOLEAN: return std::get<bool>(data) ? "true" : "false";
        case Type::VOID: return "void";
        default: return "unknown";
    }
}

Value Value::fromString(Type type, const std::string& str) {
    try {
        switch (type) {
            case Type::INT: return Value(std::stol(str));
            case Type::REAL: return Value(std::stod(str));
            case Type::STRING: return Value(str);
            case Type::BOOLEAN:
                if (str == "true") return Value(true);
                if (str == "false") return Value(false);
                throw std::runtime_error("Invalid boolean value");
            default: throw std::runtime_error("Unsupported type for conversion");
        }
    } catch (...) {
        throw std::runtime_error("Failed to convert string to value");
    }
}