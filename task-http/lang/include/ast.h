#ifndef AST_H
#define AST_H

#include "value.h"
#include "lexer.h"
#include "scope.h"
#include <vector>

/* Primary abstract classes */
class Expression {
public:
    virtual ~Expression() = default;
    virtual Value evaluate(Scope& scope) const = 0;
    virtual std::string toString() const = 0;
};

class Statement {
public:
    virtual ~Statement() = default;
    virtual void execute(Scope& scope) const = 0;
    virtual std::string toString() const = 0;
};

class Declaration {
public:
    virtual ~Declaration() = default;
    virtual void declare(Scope& scope) const = 0;
    virtual std::string toString() const = 0;
};

// Конкретные классы AST
class VariableDecl : public Declaration {
    std::string name;
    Type type;
    std::unique_ptr<Expression> initializer;
public:
    VariableDecl(
        std::string name,
        Type type,
        std::unique_ptr<Expression> init = nullptr);
    ~VariableDecl(void) override = default;
    void declare(Scope& scope) const override;
    std::string toString() const override;
};

class Identifier : public Expression {
    std::string name;
public:
    explicit Identifier(std::string name);
    ~Identifier(void) override = default;
    Value evaluate(Scope& scope) const override;
    std::string toString() const override;
};

class EnvironmentVariable : public Expression {
    std::string name;
public:
    explicit EnvironmentVariable(std::string name);
    ~EnvironmentVariable(void) override = default;
    Value evaluate(Scope& scope) const override;
    std::string toString() const override;
};

class BinaryOp : public Expression {
    TokenType op;
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;
public:
    BinaryOp(
        TokenType op,
        std::unique_ptr<Expression> left,
        std::unique_ptr<Expression> right);
    ~BinaryOp(void) override = default;
    Value evaluate(Scope& scope) const override;
    std::string toString() const override;
};

class UnaryOp: public Expression {
    TokenType op;
    std::unique_ptr<Expression> expr;
public:
    UnaryOp(TokenType op, std::unique_ptr<Expression> expr);
    ~UnaryOp(void) override = default;
    Value evaluate(Scope &scope) const override;
    std::string toString() const override;
};

class Assignment: public Expression {
    std::string name;
    std::unique_ptr<Expression> expr;
public:
    Assignment(
        std::string name,
        std::unique_ptr<Expression> expr);
    ~Assignment(void) override = default;
    Value evaluate(Scope &scope) const override;
    std::string toString() const override;
};

class CompoundStatement : public Statement {
    std::vector<std::unique_ptr<Statement>> statements;
public:
    explicit CompoundStatement(
        std::vector<std::unique_ptr<Statement>> statements);
    ~CompoundStatement(void) override = default;
    void execute(Scope& scope) const override;
    std::string toString() const override;
};

class IfStatement : public Statement {
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> thenBranch;
    std::unique_ptr<Statement> elseBranch;
public:
    IfStatement(
        std::unique_ptr<Expression> cond,
        std::unique_ptr<Statement> thenBr,
        std::unique_ptr<Statement> elseBr = nullptr);
    ~IfStatement(void) override = default;
    void execute(Scope& scope) const override;
    std::string toString() const override;
};

/* while () ... statement */
class WhileStatement: public Statement {
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> body;
public:
    WhileStatement(
        std::unique_ptr<Expression> aCondition,
        std::unique_ptr<Statement> aBody);
    ~WhileStatement(void) override = default;
    void execute(Scope &scope) const override;
    std::string toString() const override;
};

/* do ... while () statement */
class DoWhileStatement: public Statement {
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> body;
public:
    DoWhileStatement(
        std::unique_ptr<Expression> aCondition,
        std::unique_ptr<Statement> aBody);
    ~DoWhileStatement(void) override = default;
    void execute(Scope &scope) const override;
    std::string toString() const override;
};

/* for () ... statement */
class ForStatement: public Statement {
    std::unique_ptr<Expression> init;
    std::unique_ptr<Expression> cond;
    std::unique_ptr<Expression> update;
    std::unique_ptr<Statement> body;
public:
    ForStatement(
        std::unique_ptr<Expression> init,
        std::unique_ptr<Expression> cond,
        std::unique_ptr<Expression> update,
        std::unique_ptr<Statement> body);
    ~ForStatement(void) override = default;
    void execute(Scope &scope) const override;
    std::string toString() const override;
};

/* write() statement */
class WriteStatement : public Statement {
    std::vector<std::unique_ptr<Expression>> expressions;
public:
    explicit WriteStatement(
        std::vector<std::unique_ptr<Expression>> exprs);
    ~WriteStatement(void) override = default;
    void execute(Scope& scope) const override;
    std::string toString() const override;
};

/* read() statement */
class ReadStatement: public Statement {
    std::string varName;
public:
    ReadStatement(std::string varName);
    ~ReadStatement(void) override = default;
    void execute(Scope &scope) const override;
    std::string toString() const override;
};

class ExpressionStatement: public Statement {
    std::unique_ptr<Expression> expr;
public:
    ExpressionStatement(
        std::unique_ptr<Expression> expr);
    ~ExpressionStatement(void) override = default;
    void execute(Scope &scope) const override;
    std::string toString() const override;
};

/* Elementary data types: integer, double, string, boolean */
class IntegerLiteral: public Expression {
    intmax_t value;
public:
    IntegerLiteral(intmax_t value);
    ~IntegerLiteral(void) override = default;
    Value evaluate(Scope& scope) const override;
    std::string toString() const override;
};

class RealLiteral: public Expression {
    double value;
public:
    RealLiteral(double aValue);
    ~RealLiteral(void) override = default;
    Value evaluate(Scope& scope) const override;
    std::string toString() const override;
};

class StringLiteral: public Expression {
    std::string value;
public:
    StringLiteral(std::string aValue);
    ~StringLiteral(void) override = default;
    Value evaluate(Scope& scope) const override;
    std::string toString() const override;
};

class BooleanLiteral: public Expression {
    bool value;
public:
    BooleanLiteral(bool aValue);
    ~BooleanLiteral(void) override = default;
    Value evaluate(Scope &scope) const override;
    std::string toString() const override;
};

/* Program (obviously) */
class Program {
    std::vector<std::unique_ptr<Declaration>> declarations;
    std::vector<std::unique_ptr<Statement>> statements;
public:
    Program(
        std::vector<std::unique_ptr<Declaration>> decls,
        std::vector<std::unique_ptr<Statement>> stmts);
    void execute(Scope& globalScope) const;
    std::string toString() const;
};

#endif // AST_H