#ifndef SCOPE_H
#define SCOPE_H

#include "value.h"
#include <unordered_map>
#include <memory>
#include <functional>

class Scope {
public:
    using OutputFunc = std::function<void(const std::string&)>;

    Scope();
    explicit Scope(OutputFunc output);
    explicit Scope(std::shared_ptr<Scope> parent);

    void declareVariable(const std::string& name, Type type);
    void declareVariable(const std::string& name, Type type, const Value& value);
    Value getVariable(const std::string& name) const;
    void setVariable(const std::string& name, const Value& value);
    bool hasVariable(const std::string& name) const;

    void output(const std::string& text) const;
    void setOutput(OutputFunc output);

private:
    std::unordered_map<std::string, Value> variables;
    std::shared_ptr<Scope> parent;
    OutputFunc outputFunc;
};

#endif // SCOPE_H