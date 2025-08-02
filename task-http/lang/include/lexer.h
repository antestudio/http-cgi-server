#ifndef LEXER_H
#define LEXER_H

#include <string>

enum class TokenType {
    // Ключевые слова
    PROGRAM, INT, STRING, IF, ELSE, WHILE, DO, FOR, READ, WRITE,
    CASE, OF, END, STEP, UNTIL, CONTINUE, BREAK, GOTO,
    BOOLEAN, REAL, TRUE, FALSE, AND, OR, NOT,

    // Операторы и разделители
    PLUS, MINUS, MULTIPLY, DIVIDE, MODULO,
    LESS, GREATER, LESSEQUAL, GREATEREQUAL, EQUAL, NOTEQUAL,
    ASSIGN, SEMICOLON, COMMA, COLON, LPAREN, RPAREN, LBRACE, RBRACE,

    // Литералы и идентификаторы
    IDENTIFIER, INTEGER, REALNUM, STRINGLIT, ENV_VAR,

    // Специальные токены
    END_OF_FILE, ERROR
};

struct Token {
    TokenType type;
    std::string value;
    size_t line;
    size_t column;

    Token(TokenType t, const std::string& v = "", size_t l = 0, size_t c = 0)
        : type(t), value(v), line(l), column(c) {}

    bool is(TokenType t) const { return type == t; }
    bool isOneOf(TokenType t1, TokenType t2) const { return is(t1) || is(t2); }

    template <typename... Ts>
    bool isOneOf(TokenType t1, TokenType t2, Ts... ts) const {
        return is(t1) || isOneOf(t2, ts...);
    }
};

class Lexer {
public:
    Lexer(const std::string& source);

    Token nextToken();
    Token peekToken();
    void reset();

private:
    std::string source;
    size_t position;
    size_t line;
    size_t column;
    char currentChar;

    void advance();
    char peek() const;
    void skipWhitespace();
    void skipComment();
    Token readNumber();
    Token readString();
    Token readIdentifierOrKeyword();
    Token readEnvironmentVariable();
    TokenType resolveKeyword(const std::string& identifier) const;
};

#endif // LEXER_H