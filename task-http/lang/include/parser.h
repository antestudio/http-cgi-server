#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"
#include <memory>
#include <vector>

class Parser {
    Lexer& lexer;
    Token currentToken;
    Token peekToken;

public:
    Parser(Lexer& lexer);
    std::unique_ptr<Program> parse();

private:
    void nextToken();
    bool expect(TokenType type);
    bool expectPeek(TokenType type);
    [[noreturn]] void error(const std::string& message);

    // Правила грамматики
    std::unique_ptr<Program> parseProgram();
    std::vector<std::unique_ptr<Declaration>> parseDeclarations();
    std::unique_ptr<Declaration> parseDeclaration();
    Type parseType();
    std::vector<std::unique_ptr<VariableDecl>> parseVariableList(Type type);
    std::unique_ptr<VariableDecl> parseVariableDecl(Type type);
    std::unique_ptr<Expression> parseExpression();
    std::unique_ptr<Expression> parseAssignment();
    std::unique_ptr<Expression> parseOr();
    std::unique_ptr<Expression> parseAnd();
    std::unique_ptr<Expression> parseEquality();
    std::unique_ptr<Expression> parseRelational();
    std::unique_ptr<Expression> parseAdditive();
    std::unique_ptr<Expression> parseMultiplicative();
    std::unique_ptr<Expression> parseUnary();
    std::unique_ptr<Expression> parsePrimary();
    std::unique_ptr<Statement> parseStatement();
    std::unique_ptr<Statement> parseIfStatement();
    std::unique_ptr<Statement> parseWhileStatement();
    std::unique_ptr<Statement> parseDoWhileStatement();
    std::unique_ptr<Statement> parseForStatement();
    std::unique_ptr<Statement> parseCaseStatement();
    std::unique_ptr<Statement> parseLabeledStatement();
    std::unique_ptr<Statement> parseCompoundStatement();
    std::unique_ptr<Statement> parseReadStatement();
    std::unique_ptr<Statement> parseWriteStatement();
    std::unique_ptr<Statement> parseExpressionStatement();
    // std::vector<std::unique_ptr<Case>> parseCaseList();
    // std::unique_ptr<Case> parseCase();
    std::vector<std::unique_ptr<Expression>> parseExpressionList();
};

#endif // PARSER_H