#ifndef VALUE_H
#define VALUE_H

#include <string>
#include <variant>

enum class Type {
    INT, STRING, BOOLEAN, REAL, VOID
};

class Value {
public:
    Value();
    explicit Value(intmax_t val);
    explicit Value(double val);
    explicit Value(const std::string& val);
    explicit Value(bool val);

    Type getType() const;
    std::string toString() const;

    intmax_t getInt() const;
    double getReal() const;
    const std::string& getString() const;
    bool getBoolean() const;

    static Value fromString(Type type, const std::string& str);

private:
    Type type;
    std::variant<intmax_t, double, std::string, bool> data;
};

#endif // VALUE_H