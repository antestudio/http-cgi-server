#include "scope.h"
#include <stdexcept>

Scope::Scope() : parent(nullptr), outputFunc([](const std::string&) {}) {}

Scope::Scope(OutputFunc output) : parent(nullptr), outputFunc(output) {}

Scope::Scope(std::shared_ptr<Scope> parent)
    : parent(std::move(parent)), outputFunc(this->parent->outputFunc) {}

void Scope::declareVariable(const std::string& name, Type type) {
    if (variables.count(name)) {
        throw std::runtime_error("Variable '" + name + "' already declared");
    }
    variables.emplace(name, Value::fromString(type, "0"));
}

void Scope::declareVariable(const std::string& name, Type type, const Value& value) {
    if (variables.count(name)) {
        throw std::runtime_error("Variable '" + name + "' already declared");
    }
    variables.emplace(name, value);
}

Value Scope::getVariable(const std::string& name) const {
    auto it = variables.find(name);
    if (it != variables.end()) {
        return it->second;
    }
    if (parent) {
        return parent->getVariable(name);
    }
    throw std::runtime_error("Undefined variable '" + name + "'");
}

void Scope::setVariable(const std::string& name, const Value& value) {
    auto it = variables.find(name);
    if (it != variables.end()) {
        it->second = value;
        return;
    }
    if (parent) {
        parent->setVariable(name, value);
        return;
    }
    throw std::runtime_error("Undefined variable '" + name + "'");
}

bool Scope::hasVariable(const std::string& name) const {
    if (variables.count(name)) return true;
    if (parent) return parent->hasVariable(name);
    return false;
}

void Scope::output(const std::string& text) const {
    outputFunc(text);
}

void Scope::setOutput(OutputFunc output) {
    outputFunc = output;
}