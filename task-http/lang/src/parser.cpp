#include "parser.h"
#include "ast.h"
#include "lexer.h"
#include <stdexcept>
#include <string>
#include <iostream>

using namespace std;

Parser::Parser(
    Lexer& lexer
):
    lexer(lexer),
    currentToken(lexer.nextToken()),
    peekToken(lexer.nextToken())
{}

void Parser::nextToken() {
    currentToken = peekToken;
    peekToken = lexer.nextToken();
}

bool Parser::expect(TokenType type) {
    if (currentToken.type == type) {
        nextToken();
        return true;
    }
    return false;
}

bool Parser::expectPeek(TokenType type) {
    if (peekToken.type == type) {
        nextToken();
        return true;
    }
    return false;
}

[[noreturn]] void Parser::error(const string& message) {
    throw runtime_error("Parser error at line " + to_string(currentToken.line) +
                       ", column " + to_string(currentToken.column) + ": " + message);
}

unique_ptr<Program> Parser::parse() {
    return parseProgram();
}

unique_ptr<Program> Parser::parseProgram() {
    if (!expect(TokenType::PROGRAM)) {
        error("Expected 'program'");
    }

    vector<unique_ptr<Declaration>> declarations;
    vector<unique_ptr<Statement>> statements;

    while (currentToken.type != TokenType::END_OF_FILE) {
        // Парсим описания
        if (currentToken.type == TokenType::INT ||
            currentToken.type == TokenType::STRING ||
            currentToken.type == TokenType::BOOLEAN ||
            currentToken.type == TokenType::REAL) {
            auto decls = parseDeclarations();
            declarations.insert(declarations.end(),
                              make_move_iterator(decls.begin()),
                              make_move_iterator(decls.end()));
        }
        // Парсим операторы
        else {
            auto stmt = parseStatement();
            if (stmt) {
                statements.push_back(std::move(stmt));
            }
        }
    }

    return make_unique<Program>(std::move(declarations), std::move(statements));
}

vector<unique_ptr<Declaration>> Parser::parseDeclarations() {
    vector<unique_ptr<Declaration>> declarations;
    Type type = parseType();
    auto vars = parseVariableList(type);

    declarations.insert(declarations.end(),
                      make_move_iterator(vars.begin()),
                      make_move_iterator(vars.end()));

    while (expect(TokenType::SEMICOLON)) {
        if (currentToken.type == TokenType::INT ||
            currentToken.type == TokenType::STRING ||
            currentToken.type == TokenType::BOOLEAN ||
            currentToken.type == TokenType::REAL) {
            type = parseType();
            vars = parseVariableList(type);
            declarations.insert(declarations.end(),
                              make_move_iterator(vars.begin()),
                              make_move_iterator(vars.end()));
        } else {
            break;
        }
    }

    return declarations;
}

Type Parser::parseType() {
    TokenType tokType = currentToken.type;
    Type type;
    if (tokType == TokenType::INT) {
        type = Type::INT;
    }
    else if (tokType == TokenType::STRING) {
        type = Type::STRING;
    }
    else if (tokType == TokenType::REAL) {
        type = Type::REAL;
    }
    else if (tokType == TokenType::BOOLEAN) {
        type = Type::BOOLEAN;
    }
    else {
        error("Expected type (int, string, boolean or real)");
    }
    nextToken();
    return type;
}

vector<unique_ptr<VariableDecl>> Parser::parseVariableList(Type type) {
    vector<unique_ptr<VariableDecl>> vars;

    vars.push_back(parseVariableDecl(type));
    while (expect(TokenType::COMMA)) {
        vars.push_back(parseVariableDecl(type));
    }

    return vars;
}

unique_ptr<VariableDecl> Parser::parseVariableDecl(Type type) {
    if (!currentToken.is(TokenType::IDENTIFIER)) {
        error("Expected identifier");
    }

    string varName = currentToken.value;
    nextToken();

    unique_ptr<Expression> init = nullptr;
    if (expect(TokenType::ASSIGN)) {
        init = parseExpression();
    }

    return make_unique<VariableDecl>(varName, type, std::move(init));
}

unique_ptr<Expression> Parser::parseExpression() {
    return parseAssignment();
}

unique_ptr<Expression> Parser::parseAssignment() {
    auto left = parseOr();

    if (currentToken.is(TokenType::ASSIGN)) {
        if (dynamic_cast<Identifier*>(left.get()) == nullptr &&
            dynamic_cast<EnvironmentVariable*>(left.get()) == nullptr
        ) {
            error("Left side of assignment must be an identifier or an environment variable");
        }

        nextToken();
        auto right = parseAssignment();
        return make_unique<Assignment>(
            left->toString(),
            std::move(right)
        );
    }

    return left;
}

unique_ptr<Expression> Parser::parseOr() {
    auto left = parseAnd();

    while (currentToken.is(TokenType::OR)) {
        Token op = currentToken;
        nextToken();
        auto right = parseAnd();
        left = make_unique<BinaryOp>(op.type, std::move(left), std::move(right));
    }

    return left;
}

unique_ptr<Expression> Parser::parseAnd() {
    auto left = parseEquality();

    while (currentToken.is(TokenType::AND)) {
        Token op = currentToken;
        nextToken();
        auto right = parseEquality();
        left = make_unique<BinaryOp>(op.type, std::move(left), std::move(right));
    }

    return left;
}

unique_ptr<Expression> Parser::parseEquality() {
    auto left = parseRelational();

    while (currentToken.isOneOf(TokenType::EQUAL, TokenType::NOTEQUAL)) {
        Token op = currentToken;
        nextToken();
        auto right = parseRelational();
        left = make_unique<BinaryOp>(op.type, std::move(left), std::move(right));
    }

    return left;
}

unique_ptr<Expression> Parser::parseRelational() {
    auto left = parseAdditive();

    while (currentToken.isOneOf(TokenType::LESS, TokenType::GREATER,
                              TokenType::LESSEQUAL, TokenType::GREATEREQUAL)) {
        Token op = currentToken;
        nextToken();
        auto right = parseAdditive();
        left = make_unique<BinaryOp>(op.type, std::move(left), std::move(right));
    }

    return left;
}

unique_ptr<Expression> Parser::parseAdditive() {
    auto left = parseMultiplicative();

    while (currentToken.isOneOf(TokenType::PLUS, TokenType::MINUS)) {
        Token op = currentToken;
        nextToken();
        auto right = parseMultiplicative();
        left = make_unique<BinaryOp>(op.type, std::move(left), std::move(right));
    }

    return left;
}

unique_ptr<Expression> Parser::parseMultiplicative() {
    auto left = parseUnary();

    while (currentToken.isOneOf(TokenType::MULTIPLY, TokenType::DIVIDE, TokenType::MODULO)) {
        Token op = currentToken;
        nextToken();
        auto right = parseUnary();
        left = make_unique<BinaryOp>(op.type, std::move(left), std::move(right));
    }

    return left;
}

unique_ptr<Expression> Parser::parseUnary() {
    if (currentToken.isOneOf(TokenType::MINUS, TokenType::NOT)) {
        Token op = currentToken;
        nextToken();
        auto expr = parseUnary();
        return make_unique<UnaryOp>(op.type, std::move(expr));
    }
    return parsePrimary();
}

unique_ptr<Expression> Parser::parsePrimary() {
    Token token = currentToken;

    switch (token.type) {
        case TokenType::INTEGER:
            nextToken();
            return make_unique<IntegerLiteral>(stoi(token.value));

        case TokenType::REALNUM:
            nextToken();
            return make_unique<RealLiteral>(stod(token.value));

        case TokenType::STRINGLIT:
            nextToken();
            return make_unique<StringLiteral>(token.value);

        case TokenType::TRUE:
            nextToken();
            return make_unique<BooleanLiteral>(true);

        case TokenType::FALSE:
            nextToken();
            return make_unique<BooleanLiteral>(false);

        case TokenType::IDENTIFIER: {
            string name = token.value;
            nextToken();
            return make_unique<Identifier>(name);
        }

        case TokenType::ENV_VAR: {
            string name = token.value;
            nextToken();
            return make_unique<EnvironmentVariable>(name);
        }

        case TokenType::LPAREN: {
            nextToken();
            auto expr = parseExpression();
            if (!expect(TokenType::RPAREN)) {
                error("Expected ')' after expression");
            }
            return expr;
        }

        default:
            error("Unexpected token in expression: " + token.value);
            return nullptr;
    }
}

unique_ptr<Statement> Parser::parseStatement() {
    switch (currentToken.type) {
        case TokenType::LBRACE:
            return parseCompoundStatement();

        case TokenType::IF:
            return parseIfStatement();

        case TokenType::WHILE:
            return parseWhileStatement();

        case TokenType::DO:
            return parseDoWhileStatement();

        case TokenType::FOR:
            return parseForStatement();

        case TokenType::READ:
            return parseReadStatement();

        case TokenType::WRITE:
            return parseWriteStatement();

        case TokenType::IDENTIFIER:
            if (peekToken.type == TokenType::COLON) {
                return parseLabeledStatement();
            }
            // Продолжаем как выражение

        default:
            return parseExpressionStatement();
    }
}

unique_ptr<Statement> Parser::parseCompoundStatement() {
    if (!expect(TokenType::LBRACE)) {
        error("Expected '{'");
    }

    vector<unique_ptr<Statement>> statements;
    while (!currentToken.is(TokenType::RBRACE)) {
        auto stmt = parseStatement();
        if (stmt) {
            statements.push_back(std::move(stmt));
        }
    }

    if (!expect(TokenType::RBRACE)) {
        error("Expected '}' after compound statement");
    }

    return make_unique<CompoundStatement>(std::move(statements));
}

unique_ptr<Statement> Parser::parseIfStatement() {
    if (!expect(TokenType::IF)) {
        error("Expected 'if'");
    }

    if (!expect(TokenType::LPAREN)) {
        error("Expected '(' after 'if'");
    }

    auto condition = parseExpression();

    if (!expect(TokenType::RPAREN)) {
        error("Expected ')' after if condition");
    }

    auto thenBranch = parseStatement();

    unique_ptr<Statement> elseBranch = nullptr;
    if (currentToken.is(TokenType::ELSE)) {
        nextToken();
        elseBranch = parseStatement();
    }

    return make_unique<IfStatement>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

unique_ptr<Statement> Parser::parseWhileStatement() {
    if (!expect(TokenType::WHILE)) {
        error("Expected 'while'");
    }

    if (!expect(TokenType::LPAREN)) {
        error("Expected '(' after 'while'");
    }

    auto condition = parseExpression();

    if (!expect(TokenType::RPAREN)) {
        error("Expected ')' after while condition");
    }

    auto body = parseStatement();

    return make_unique<WhileStatement>(std::move(condition), std::move(body));
}

unique_ptr<Statement> Parser::parseDoWhileStatement() {
    if (!expect(TokenType::DO)) {
        error("Expected 'do'");
    }

    auto body = parseStatement();

    if (!expect(TokenType::WHILE)) {
        error("Expected 'while' after do statement");
    }

    if (!expect(TokenType::LPAREN)) {
        error("Expected '(' after 'while'");
    }

    auto condition = parseExpression();

    if (!expect(TokenType::RPAREN)) {
        error("Expected ')' after while condition");
    }

    if (!expect(TokenType::SEMICOLON)) {
        error("Expected ';' after do-while statement");
    }

    return make_unique<DoWhileStatement>(
        std::move(condition),
        std::move(body)
    );
}

unique_ptr<Statement> Parser::parseForStatement() {
    if (!expect(TokenType::FOR)) {
        error("Expected 'for'");
    }

    if (!expect(TokenType::LPAREN)) {
        error("Expected '(' after 'for'");
    }

    // Инициализация
    unique_ptr<Expression> init = nullptr;
    if (!currentToken.is(TokenType::SEMICOLON)) {
        init = parseExpression();
    }

    if (!expect(TokenType::SEMICOLON)) {
        error("Expected ';' after for init");
    }

    // Условие
    unique_ptr<Expression> condition = nullptr;
    if (!currentToken.is(TokenType::SEMICOLON)) {
        condition = parseExpression();
    }

    if (!expect(TokenType::SEMICOLON)) {
        error("Expected ';' after for condition");
    }

    // Обновление
    unique_ptr<Expression> update = nullptr;
    if (!currentToken.is(TokenType::RPAREN)) {
        update = parseExpression();
    }

    if (!expect(TokenType::RPAREN)) {
        error("Expected ')' after for update");
    }

    auto body = parseStatement();

    return make_unique<ForStatement>(std::move(init), std::move(condition), std::move(update), std::move(body));
}

unique_ptr<Statement> Parser::parseReadStatement() {
    if (!expect(TokenType::READ)) {
        error("Expected 'read'");
    }

    if (!expect(TokenType::LPAREN)) {
        error("Expected '(' after 'read'");
    }

    if (!currentToken.is(TokenType::IDENTIFIER)) {
        error("Expected identifier in read statement");
    }

    string varName = currentToken.value;
    nextToken();

    if (!expect(TokenType::RPAREN)) {
        error("Expected ')' after read variable");
    }

    if (!expect(TokenType::SEMICOLON)) {
        error("Expected ';' after read statement");
    }

    return make_unique<ReadStatement>(varName);
}

unique_ptr<Statement> Parser::parseWriteStatement() {
    if (!expect(TokenType::WRITE)) {
        error("Expected 'write'");
    }

    if (!expect(TokenType::LPAREN)) {
        error("Expected '(' after 'write'");
    }

    auto expressions = parseExpressionList();

    if (!expect(TokenType::RPAREN)) {
        error("Expected ')' after write arguments");
    }

    if (!expect(TokenType::SEMICOLON)) {
        error("Expected ';' after write statement");
    }

    return make_unique<WriteStatement>(std::move(expressions));
}

unique_ptr<Statement> Parser::parseLabeledStatement() {
    string label = currentToken.value;
    nextToken(); // Идентификатор
    nextToken(); // Двоеточие

    auto stmt = parseStatement();

    // В данной реализации метки не используются, просто пропускаем
    return stmt;
}

unique_ptr<Statement> Parser::parseExpressionStatement() {
    auto expr = parseExpression();
    if (!expect(TokenType::SEMICOLON)) {
        error("Expected ';' after expression");
    }
    return make_unique<ExpressionStatement>(std::move(expr));
}

vector<unique_ptr<Expression>> Parser::parseExpressionList() {
    vector<unique_ptr<Expression>> exprs;

    if (!currentToken.is(TokenType::RPAREN)) {
        exprs.push_back(parseExpression());
        while (expect(TokenType::COMMA)) {
            exprs.push_back(parseExpression());
        }
    }

    return exprs;
}