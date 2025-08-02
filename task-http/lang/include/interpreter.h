#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "scope.h"
#include <functional>

class Interpreter {
public:
    using OutputFunc = std::function<void(const std::string&)>;

    Interpreter();
    explicit Interpreter(OutputFunc output);

    void interpret(const std::string& source);
    void setOutput(OutputFunc output);

private:
    OutputFunc output;
    std::unique_ptr<Scope> global;
};

#endif // INTERPRETER_H