#include "ast.h"
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <memory>
#include <vector>

using namespace std;

// =============================================
// Реализация VariableDecl
// =============================================

VariableDecl::VariableDecl(string name, Type type, unique_ptr<Expression> init)
    : name(std::move(name)), type(type), initializer(std::move(init)) {}

void VariableDecl::declare(Scope& scope) const {
    if (initializer) {
        Value val = initializer->evaluate(scope);
        if (type == Type::INT && val.getType() != Type::INT) {
            throw runtime_error("Type mismatch in initialization of variable '" + name + "'");
        }
        if (type == Type::STRING && val.getType() != Type::STRING) {
            throw runtime_error("Type mismatch in initialization of variable '" + name + "'");
        }
        scope.declareVariable(name, type, val);
    } else {
        // Инициализация значением по умолчанию
        Value defaultValue;
        switch (type) {
            case Type::INT: defaultValue = Value(0L); break;
            case Type::REAL: defaultValue = Value(0.0); break;
            case Type::STRING: defaultValue = Value(""); break;
            case Type::BOOLEAN: defaultValue = Value(false); break;
            default: throw runtime_error("Unknown type for variable '" + name + "'");
        }
        scope.declareVariable(name, type, defaultValue);
    }
}

string VariableDecl::toString() const {
    string typeStr;
    switch (type) {
        case Type::INT: typeStr = "int"; break;
        case Type::STRING: typeStr = "string"; break;
        case Type::BOOLEAN: typeStr = "boolean"; break;
        case Type::REAL: typeStr = "real"; break;
        default: typeStr = "unknown"; break;
    }

    string initStr = initializer ? " = " + initializer->toString() : "";
    return typeStr + " " + name + initStr;
}

// =============================================
// Реализация Identifier
// =============================================

Identifier::Identifier(string name) : name(std::move(name)) {}

Value Identifier::evaluate(Scope& scope) const {
    return scope.getVariable(name);
}

string Identifier::toString() const {
    return name;
}

// =============================================
// Реализация EnvironmentVariable
// =============================================

EnvironmentVariable::EnvironmentVariable(string name) : name(std::move(name)) {}

Value EnvironmentVariable::evaluate(Scope& scope) const {
    const char* env = getenv(name.c_str());
    if (!env) {
        return Value(std::string("")); // Возвращаем пустую строку, если переменная не определена
    }
    return Value(std::string(env));
}

string EnvironmentVariable::toString() const {
    return "$" + name;
}

// =============================================
// Реализация IntegerLiteral
// =============================================

IntegerLiteral::IntegerLiteral(intmax_t aValue):
    value(aValue)
{}

Value IntegerLiteral::evaluate(Scope& scope) const {
    return Value(value);
}

string IntegerLiteral::toString() const {
    return to_string(value);
}

// =============================================
// Реализация RealLiteral
// =============================================

RealLiteral::RealLiteral(double aValue):
    value(aValue)
{}

Value RealLiteral::evaluate(Scope& scope) const {
    return Value(value);
}

string RealLiteral::toString() const {
    return to_string(value);
}

// =============================================
// Реализация StringLiteral
// =============================================

StringLiteral::StringLiteral(string aValue):
    value(std::move(aValue))
{}

Value StringLiteral::evaluate(Scope& scope) const {
    return Value(value);
}

string StringLiteral::toString() const {
    return "\"" + value + "\"";
}

// =============================================
// Реализация BooleanLiteral
// =============================================

BooleanLiteral::BooleanLiteral(bool value) : value(value) {}

Value BooleanLiteral::evaluate(Scope& scope) const {
    return Value(value);
}

string BooleanLiteral::toString() const {
    return value ? "true" : "false";
}

// =============================================
// Реализация BinaryOp
// =============================================

BinaryOp::BinaryOp(TokenType op, unique_ptr<Expression> left, unique_ptr<Expression> right)
    : op(op), left(std::move(left)), right(std::move(right)) {}

Value BinaryOp::evaluate(Scope& scope) const {
    Value leftVal = left->evaluate(scope);
    Value rightVal = right->evaluate(scope);

    // Арифметические операции
    switch (op) {
        case TokenType::PLUS:
            if (leftVal.getType() == Type::INT && rightVal.getType() == Type::INT) {
                return Value(leftVal.getInt() + rightVal.getInt());
            }
            if (leftVal.getType() == Type::REAL && rightVal.getType() == Type::REAL) {
                return Value(leftVal.getReal() + rightVal.getReal());
            }
            if (leftVal.getType() == Type::REAL && rightVal.getType() == Type::INT) {
                return Value(leftVal.getReal() + rightVal.getInt());
            }
            if (leftVal.getType() == Type::INT && rightVal.getType() == Type::REAL) {
                return Value(leftVal.getInt() + rightVal.getReal());
            }
            if (leftVal.getType() == Type::STRING && rightVal.getType() == Type::STRING) {
                return Value(leftVal.getString() + rightVal.getString());
            }
            break;

        case TokenType::MINUS:
            if (leftVal.getType() == Type::INT && rightVal.getType() == Type::INT) {
                return Value(leftVal.getInt() - rightVal.getInt());
            }
            if (leftVal.getType() == Type::REAL && rightVal.getType() == Type::REAL) {
                return Value(leftVal.getReal() - rightVal.getReal());
            }
            if (leftVal.getType() == Type::REAL && rightVal.getType() == Type::INT) {
                return Value(leftVal.getReal() - rightVal.getInt());
            }
            if (leftVal.getType() == Type::INT && rightVal.getType() == Type::REAL) {
                return Value(leftVal.getInt() - rightVal.getReal());
            }
            break;

        case TokenType::MULTIPLY:
            if (leftVal.getType() == Type::INT && rightVal.getType() == Type::INT) {
                return Value(leftVal.getInt() * rightVal.getInt());
            }
            if (leftVal.getType() == Type::REAL && rightVal.getType() == Type::REAL) {
                return Value(leftVal.getReal() * rightVal.getReal());
            }
            if (leftVal.getType() == Type::REAL && rightVal.getType() == Type::INT) {
                return Value(leftVal.getReal() * rightVal.getInt());
            }
            if (leftVal.getType() == Type::INT && rightVal.getType() == Type::REAL) {
                return Value(leftVal.getInt() * rightVal.getReal());
            }
            break;

        case TokenType::DIVIDE:
            if (rightVal.getInt() == 0 || rightVal.getReal() == 0.0) {
                throw runtime_error("Division by zero");
            }
            if (leftVal.getType() == Type::INT && rightVal.getType() == Type::INT) {
                return Value(leftVal.getInt() / rightVal.getInt());
            }
            if (leftVal.getType() == Type::REAL && rightVal.getType() == Type::REAL) {
                return Value(leftVal.getReal() / rightVal.getReal());
            }
            if (leftVal.getType() == Type::REAL && rightVal.getType() == Type::INT) {
                return Value(leftVal.getReal() / rightVal.getInt());
            }
            if (leftVal.getType() == Type::INT && rightVal.getType() == Type::REAL) {
                return Value(leftVal.getInt() / rightVal.getReal());
            }
            break;

        case TokenType::MODULO:
            if (leftVal.getType() == Type::INT && rightVal.getType() == Type::INT) {
                if (rightVal.getInt() == 0) {
                    throw runtime_error("Modulo by zero");
                }
                return Value(leftVal.getInt() % rightVal.getInt());
            }
            break;

        // Операции сравнения
        case TokenType::LESS:
            if (leftVal.getType() == Type::INT && rightVal.getType() == Type::INT) {
                return Value(leftVal.getInt() < rightVal.getInt());
            }
            if (leftVal.getType() == Type::REAL && rightVal.getType() == Type::REAL) {
                return Value(leftVal.getReal() < rightVal.getReal());
            }
            if (leftVal.getType() == Type::REAL && rightVal.getType() == Type::INT) {
                return Value(leftVal.getReal() < rightVal.getInt());
            }
            if (leftVal.getType() == Type::INT && rightVal.getType() == Type::REAL) {
                return Value(leftVal.getInt() < rightVal.getReal());
            }
            if (leftVal.getType() == Type::STRING && rightVal.getType() == Type::STRING) {
                return Value(leftVal.getString() < rightVal.getString());
            }
            break;

        case TokenType::GREATER:
            if (leftVal.getType() == Type::INT && rightVal.getType() == Type::INT) {
                return Value(leftVal.getInt() > rightVal.getInt());
            }
            if (leftVal.getType() == Type::REAL && rightVal.getType() == Type::REAL) {
                return Value(leftVal.getReal() > rightVal.getReal());
            }
            if (leftVal.getType() == Type::REAL && rightVal.getType() == Type::INT) {
                return Value(leftVal.getReal() > rightVal.getInt());
            }
            if (leftVal.getType() == Type::INT && rightVal.getType() == Type::REAL) {
                return Value(leftVal.getInt() > rightVal.getReal());
            }
            if (leftVal.getType() == Type::STRING && rightVal.getType() == Type::STRING) {
                return Value(leftVal.getString() > rightVal.getString());
            }
            break;

        case TokenType::LESSEQUAL:
            if (leftVal.getType() == Type::INT && rightVal.getType() == Type::INT) {
                return Value(leftVal.getInt() <= rightVal.getInt());
            }
            if (leftVal.getType() == Type::REAL && rightVal.getType() == Type::REAL) {
                return Value(leftVal.getReal() <= rightVal.getReal());
            }
            if (leftVal.getType() == Type::REAL && rightVal.getType() == Type::INT) {
                return Value(leftVal.getReal() <= rightVal.getInt());
            }
            if (leftVal.getType() == Type::INT && rightVal.getType() == Type::REAL) {
                return Value(leftVal.getInt() <= rightVal.getReal());
            }
            if (leftVal.getType() == Type::STRING && rightVal.getType() == Type::STRING) {
                return Value(leftVal.getString() <= rightVal.getString());
            }
            break;

        case TokenType::GREATEREQUAL:
            if (leftVal.getType() == Type::INT && rightVal.getType() == Type::INT) {
                return Value(leftVal.getInt() >= rightVal.getInt());
            }
            if (leftVal.getType() == Type::REAL && rightVal.getType() == Type::REAL) {
                return Value(leftVal.getReal() >= rightVal.getReal());
            }
            if (leftVal.getType() == Type::REAL && rightVal.getType() == Type::INT) {
                return Value(leftVal.getReal() >= rightVal.getInt());
            }
            if (leftVal.getType() == Type::INT && rightVal.getType() == Type::REAL) {
                return Value(leftVal.getInt() >= rightVal.getReal());
            }
            if (leftVal.getType() == Type::STRING && rightVal.getType() == Type::STRING) {
                return Value(leftVal.getString() >= rightVal.getString());
            }
            break;

        case TokenType::EQUAL:
            if (leftVal.getType() == Type::INT && rightVal.getType() == Type::INT) {
                return Value(leftVal.getInt() == rightVal.getInt());
            }
            if (leftVal.getType() == Type::REAL && rightVal.getType() == Type::REAL) {
                return Value(leftVal.getReal() == rightVal.getReal());
            }
            if (leftVal.getType() == Type::REAL && rightVal.getType() == Type::INT) {
                return Value(leftVal.getReal() == rightVal.getInt());
            }
            if (leftVal.getType() == Type::INT && rightVal.getType() == Type::REAL) {
                return Value(leftVal.getInt() == rightVal.getReal());
            }
            if (leftVal.getType() == Type::STRING && rightVal.getType() == Type::STRING) {
                return Value(leftVal.getString() == rightVal.getString());
            }
            if (leftVal.getType() == Type::BOOLEAN && rightVal.getType() == Type::BOOLEAN) {
                return Value(leftVal.getBoolean() == rightVal.getBoolean());
            }
            break;

        case TokenType::NOTEQUAL:
            if (leftVal.getType() == Type::INT && rightVal.getType() == Type::INT) {
                return Value(leftVal.getInt() != rightVal.getInt());
            }
            if (leftVal.getType() == Type::REAL && rightVal.getType() == Type::REAL) {
                return Value(leftVal.getReal() != rightVal.getReal());
            }
            if (leftVal.getType() == Type::REAL && rightVal.getType() == Type::INT) {
                return Value(leftVal.getReal() != rightVal.getInt());
            }
            if (leftVal.getType() == Type::INT && rightVal.getType() == Type::REAL) {
                return Value(leftVal.getInt() != rightVal.getReal());
            }
            if (leftVal.getType() == Type::STRING && rightVal.getType() == Type::STRING) {
                return Value(leftVal.getString() != rightVal.getString());
            }
            if (leftVal.getType() == Type::BOOLEAN && rightVal.getType() == Type::BOOLEAN) {
                return Value(leftVal.getBoolean() != rightVal.getBoolean());
            }
            break;

        // Логические операции
        case TokenType::AND:
            if (leftVal.getType() == Type::BOOLEAN && rightVal.getType() == Type::BOOLEAN) {
                return Value(leftVal.getBoolean() && rightVal.getBoolean());
            }
            break;

        case TokenType::OR:
            if (leftVal.getType() == Type::BOOLEAN && rightVal.getType() == Type::BOOLEAN) {
                return Value(leftVal.getBoolean() || rightVal.getBoolean());
            }
            break;

        default:
            break;
    }

    throw runtime_error("Unsupported binary operation between types");
}

string BinaryOp::toString() const {
    string opStr;
    switch (op) {
        case TokenType::PLUS: opStr = "+"; break;
        case TokenType::MINUS: opStr = "-"; break;
        case TokenType::MULTIPLY: opStr = "*"; break;
        case TokenType::DIVIDE: opStr = "/"; break;
        case TokenType::MODULO: opStr = "%"; break;
        case TokenType::LESS: opStr = "<"; break;
        case TokenType::GREATER: opStr = ">"; break;
        case TokenType::LESSEQUAL: opStr = "<="; break;
        case TokenType::GREATEREQUAL: opStr = ">="; break;
        case TokenType::EQUAL: opStr = "=="; break;
        case TokenType::NOTEQUAL: opStr = "!="; break;
        case TokenType::AND: opStr = "and"; break;
        case TokenType::OR: opStr = "or"; break;
        default: opStr = "??"; break;
    }

    return "(" + left->toString() + " " + opStr + " " + right->toString() + ")";
}

// =============================================
// Реализация UnaryOp
// =============================================

UnaryOp::UnaryOp(TokenType op, unique_ptr<Expression> expr)
    : op(op), expr(std::move(expr)) {}

Value UnaryOp::evaluate(Scope& scope) const {
    Value val = expr->evaluate(scope);

    switch (op) {
        case TokenType::MINUS:
            if (val.getType() == Type::INT) {
                return Value(-val.getInt());
            }
            if (val.getType() == Type::REAL) {
                return Value(-val.getReal());
            }
            break;

        case TokenType::NOT:
            if (val.getType() == Type::BOOLEAN) {
                return Value(!val.getBoolean());
            }
            break;

        default:
            break;
    }

    throw runtime_error("Unsupported unary operation for type");
}

string UnaryOp::toString() const {
    string opStr;
    switch (op) {
        case TokenType::MINUS: opStr = "-"; break;
        case TokenType::NOT: opStr = "not"; break;
        default: opStr = "??"; break;
    }

    return opStr + " " + expr->toString();
}

// =============================================
// Реализация Assignment
// =============================================

Assignment::Assignment(
    string name,
    unique_ptr<Expression> expr
):
    name(std::move(name)),
    expr(std::move(expr))
{}

Value Assignment::evaluate(Scope& scope) const {
    Value val = expr->evaluate(scope);
    scope.setVariable(name, val);
    return val;
}

string Assignment::toString() const {
    return name + " = " + expr->toString();
}

// =============================================
// Реализация CompoundStatement
// =============================================

CompoundStatement::CompoundStatement(vector<unique_ptr<Statement>> statements)
    : statements(std::move(statements)) {}

void CompoundStatement::execute(Scope& scope) const {
    for (const auto& stmt : statements) {
        stmt->execute(scope);
    }
}

string CompoundStatement::toString() const {
    string result = "{\n";
    for (const auto& stmt : statements) {
        result += "  " + stmt->toString() + "\n";
    }
    return result + "}";
}

// =============================================
// Реализация IfStatement
// =============================================

IfStatement::IfStatement(
    unique_ptr<Expression> cond,
    unique_ptr<Statement> thenBr,
    unique_ptr<Statement> elseBr
):
    condition(std::move(cond)),
    thenBranch(std::move(thenBr)),
    elseBranch(std::move(elseBr))
{}

void IfStatement::execute(Scope& scope) const {
    Value condVal = condition->evaluate(scope);
    if (condVal.getBoolean()) {
        thenBranch->execute(scope);
    } else if (elseBranch) {
        elseBranch->execute(scope);
    }
}

string IfStatement::toString() const {
    string elseStr = elseBranch ? " else " + elseBranch->toString() : "";
    return "if (" + condition->toString() + ") " + thenBranch->toString() + elseStr;
}

// =============================================
// Реализация WhileStatement
// =============================================

WhileStatement::WhileStatement(
    unique_ptr<Expression> cond,
    unique_ptr<Statement> body
):
    condition(std::move(cond)),
    body(std::move(body))
{}

void WhileStatement::execute(Scope& scope) const {
    while (condition->evaluate(scope).getBoolean()) {
        body->execute(scope);
    }
}

string WhileStatement::toString() const {
    return "while (" + condition->toString() + ") " + body->toString();
}

// =============================================
// Реализация DoWhileStatement
// =============================================

DoWhileStatement::DoWhileStatement(
    unique_ptr<Expression> cond,
    unique_ptr<Statement> body
):
    condition(std::move(cond)),
    body(std::move(body))
{}

void DoWhileStatement::execute(Scope& scope) const {
    do {
        body->execute(scope);
    } while (condition->evaluate(scope).getBoolean());
}

string DoWhileStatement::toString() const {
    return "do " + body->toString() + " while (" + condition->toString() + ");";
}

// =============================================
// Реализация ForStatement
// =============================================

ForStatement::ForStatement(
    unique_ptr<Expression> init,
    unique_ptr<Expression> cond,
    unique_ptr<Expression> update,
    unique_ptr<Statement> body
):
    init(std::move(init)),
    cond(std::move(cond)),
    update(std::move(update)),
    body(std::move(body))
{}

void ForStatement::execute(Scope& scope) const {
    if (init) {
        init->evaluate(scope);
    }

    while (cond ? cond->evaluate(scope).getBoolean() : true) {
        body->execute(scope);
        if (update) {
            update->evaluate(scope);
        }
    }
}

string ForStatement::toString() const {
    string initStr = init ? init->toString() : "";
    string condStr = cond ? cond->toString() : "";
    string updateStr = update ? update->toString() : "";
    return "for (" + initStr + "; " + condStr + "; " + updateStr + ") " + body->toString();
}

// =============================================
// Реализация WriteStatement
// =============================================

WriteStatement::WriteStatement(vector<unique_ptr<Expression>> exprs)
    : expressions(std::move(exprs)) {}

void WriteStatement::execute(Scope& scope) const {
    for (const auto& expr : expressions) {
        Value val = expr->evaluate(scope);
        scope.output(val.toString());
    }
}

string WriteStatement::toString() const {
    string result = "write(";
    for (size_t i = 0; i < expressions.size(); ++i) {
        if (i > 0) result += ", ";
        result += expressions[i]->toString();
    }
    return result + ");";
}

// =============================================
// Реализация ReadStatement
// =============================================

ReadStatement::ReadStatement(string varName)
    : varName(std::move(varName)) {}

void ReadStatement::execute(Scope& scope) const {
    string input;
    getline(std::cin, input);

    // Определяем тип переменной и преобразуем ввод
    Value currentValue = scope.getVariable(varName);

    Value newValue;
    switch (currentValue.getType()) {
        case Type::INT:
            try {
                newValue = Value(std::stol(input));
            } catch (...) {
                throw runtime_error("Invalid integer input for variable '" + varName + "'");
            }
            break;

        case Type::REAL:
            try {
                newValue = Value(std::stod(input));
            } catch (...) {
                throw runtime_error("Invalid real number input for variable '" + varName + "'");
            }
            break;

        case Type::STRING:
            newValue = Value(input);
            break;

        case Type::BOOLEAN:
            if (input == "true" || input == "1") {
                newValue = Value(true);
            } else if (input == "false" || input == "0") {
                newValue = Value(false);
            } else {
                throw runtime_error("Invalid boolean input for variable '" + varName + "'");
            }
            break;

        default:
            throw runtime_error("Unsupported type for read operation");
    }

    scope.setVariable(varName, newValue);
}

string ReadStatement::toString() const {
    return "read(" + varName + ");";
}

// =============================================
// Реализация ExpressionStatement
// =============================================

ExpressionStatement::ExpressionStatement(
    unique_ptr<Expression> expr
):
    expr(std::move(expr))
{}

void ExpressionStatement::execute(Scope& scope) const {
    expr->evaluate(scope);
}

string ExpressionStatement::toString() const {
    return expr->toString() + ";";
}

// =============================================
// Реализация Program
// =============================================

Program::Program(
    vector<unique_ptr<Declaration>> decls,
    vector<unique_ptr<Statement>> stmts
):
    declarations(std::move(decls)),
    statements(std::move(stmts))
{}

void Program::execute(Scope& globalScope) const {
    // Объявление переменных
    for (const auto& decl : declarations) {
        decl->declare(globalScope);
    }

    // Выполнение операторов
    for (const auto& stmt : statements) {
        stmt->execute(globalScope);
    }
}

string Program::toString() const {
    string result = "program {\n";

    // Декларации
    for (const auto& decl : declarations) {
        result += "  " + decl->toString() + ";\n";
    }

    // Операторы
    for (const auto& stmt : statements) {
        result += "  " + stmt->toString() + "\n";
    }

    return result + "}";
}