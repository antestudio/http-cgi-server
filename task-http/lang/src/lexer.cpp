#include "lexer.h"
#include <cctype>
#include <unordered_map>

using namespace std;

static const unordered_map<string, TokenType> keywords = {
    {"program", TokenType::PROGRAM},
    {"int", TokenType::INT},
    {"string", TokenType::STRING},
    {"if", TokenType::IF},
    {"else", TokenType::ELSE},
    {"while", TokenType::WHILE},
    {"do", TokenType::DO},
    {"for", TokenType::FOR},
    {"read", TokenType::READ},
    {"write", TokenType::WRITE},
    {"case", TokenType::CASE},
    {"of", TokenType::OF},
    {"end", TokenType::END},
    {"step", TokenType::STEP},
    {"until", TokenType::UNTIL},
    {"continue", TokenType::CONTINUE},
    {"break", TokenType::BREAK},
    {"goto", TokenType::GOTO},
    {"boolean", TokenType::BOOLEAN},
    {"real", TokenType::REAL},
    {"true", TokenType::TRUE},
    {"false", TokenType::FALSE},
    {"and", TokenType::AND},
    {"or", TokenType::OR},
    {"not", TokenType::NOT}
};

Lexer::Lexer(const string& source)
    : source(source), position(0), line(1), column(0) {
    currentChar = source.empty() ? '\0' : source[0];
}

void Lexer::advance() {
    if (currentChar == '\n') {
        line++;
        column = 0;
    } else {
        column++;
    }

    position++;
    currentChar = position < source.size() ? source[position] : '\0';
}

char Lexer::peek() const {
    return position + 1 < source.size() ? source[position + 1] : '\0';
}

void Lexer::skipWhitespace() {
    while (isspace(currentChar)) {
        advance();
    }
}

void Lexer::skipComment() {
    if (currentChar == '/' && peek() == '*') {
        advance(); // /
        advance(); // *

        while (currentChar != '\0' && !(currentChar == '*' && peek() == '/')) {
            advance();
        }

        if (currentChar != '\0') {
            advance(); // *
            advance(); // /
        }
    }
}

Token Lexer::readNumber() {
    string number;
    bool isReal = false;

    // Знак
    if (currentChar == '+' || currentChar == '-') {
        number += currentChar;
        advance();
    }

    // Целая часть
    while (isdigit(currentChar)) {
        number += currentChar;
        advance();
    }

    // Дробная часть
    if (currentChar == '.') {
        isReal = true;
        number += currentChar;
        advance();

        while (isdigit(currentChar)) {
            number += currentChar;
            advance();
        }
    }

    // Экспоненциальная часть
    if (currentChar == 'e' || currentChar == 'E') {
        isReal = true;
        number += currentChar;
        advance();

        if (currentChar == '+' || currentChar == '-') {
            number += currentChar;
            advance();
        }

        while (isdigit(currentChar)) {
            number += currentChar;
            advance();
        }
    }

    return Token(isReal ? TokenType::REALNUM : TokenType::INTEGER, number, line, column);
}

Token Lexer::readString() {
    string str;
    size_t startLine = line;
    size_t startCol = column;

    advance(); // Пропускаем открывающую кавычку

    while (currentChar != '"' && currentChar != '\0') {
        if (currentChar == '\\') {
            advance();
            switch (currentChar) {
                case 'n': str += '\n'; break;
                case 't': str += '\t'; break;
                case '"': str += '"'; break;
                case '\\': str += '\\'; break;
                default: str += '\\'; str += currentChar; break;
            }
        } else {
            str += currentChar;
        }
        advance();
    }

    if (currentChar != '"') {
        return Token(TokenType::ERROR, "Unterminated string", startLine, startCol);
    }

    advance(); // Пропускаем закрывающую кавычку
    return Token(TokenType::STRINGLIT, str, line, column);
}

Token Lexer::readIdentifierOrKeyword() {
    string ident;
    size_t startLine = line;
    size_t startCol = column;

    while (isalnum(currentChar) || currentChar == '_') {
        ident += currentChar;
        advance();
    }

    TokenType type = resolveKeyword(ident);
    if (type != TokenType::IDENTIFIER) {
        return Token(type, ident, startLine, startCol);
    }

    return Token(TokenType::IDENTIFIER, ident, startLine, startCol);
}

Token Lexer::readEnvironmentVariable() {
    size_t startLine = line;
    size_t startCol = column;

    advance(); // Пропускаем $
    string varName;

    while (isalnum(currentChar) || currentChar == '_') {
        varName += currentChar;
        advance();
    }

    if (varName.empty()) {
        return Token(TokenType::ERROR, "Invalid environment variable name", startLine, startCol);
    }

    return Token(TokenType::ENV_VAR, varName, startLine, startCol);
}

TokenType Lexer::resolveKeyword(const string& identifier) const {
    auto it = keywords.find(identifier);
    return it != keywords.end() ? it->second : TokenType::IDENTIFIER;
}

Token Lexer::nextToken() {
    skipWhitespace();
    skipComment();
    skipWhitespace();

    if (currentChar == '\0') {
        return Token(TokenType::END_OF_FILE, "", line, column);
    }

    // Числа
    if (isdigit(currentChar) ||
        ((currentChar == '+' || currentChar == '-') && isdigit(peek()))) {
        return readNumber();
    }

    // Строки
    if (currentChar == '"') {
        return readString();
    }

    // Переменные окружения
    if (currentChar == '$') {
        return readEnvironmentVariable();
    }

    // Идентификаторы и ключевые слова
    if (isalpha(currentChar) || currentChar == '_') {
        return readIdentifierOrKeyword();
    }

    // Операторы и разделители
    size_t currentLine = line;
    size_t currentCol = column;

    switch (currentChar) {
        case '+':
            advance();
            return Token(TokenType::PLUS, "+", currentLine, currentCol);
        case '-':
            advance();
            return Token(TokenType::MINUS, "-", currentLine, currentCol);
        case '*':
            advance();
            return Token(TokenType::MULTIPLY, "*", currentLine, currentCol);
        case '/':
            advance();
            return Token(TokenType::DIVIDE, "/", currentLine, currentCol);
        case '%':
            advance();
            return Token(TokenType::MODULO, "%", currentLine, currentCol);
        case '=':
            advance();
            if (currentChar == '=') {
                advance();
                return Token(TokenType::EQUAL, "==", currentLine, currentCol);
            }
            return Token(TokenType::ASSIGN, "=", currentLine, currentCol);
        case '<':
            advance();
            if (currentChar == '=') {
                advance();
                return Token(TokenType::LESSEQUAL, "<=", currentLine, currentCol);
            }
            return Token(TokenType::LESS, "<", currentLine, currentCol);
        case '>':
            advance();
            if (currentChar == '=') {
                advance();
                return Token(TokenType::GREATEREQUAL, ">=", currentLine, currentCol);
            }
            return Token(TokenType::GREATER, ">", currentLine, currentCol);
        case '!':
            advance();
            if (currentChar == '=') {
                advance();
                return Token(TokenType::NOTEQUAL, "!=", currentLine, currentCol);
            }
            return Token(TokenType::ERROR, "Unexpected character '!'", currentLine, currentCol);
        case ';':
            advance();
            return Token(TokenType::SEMICOLON, ";", currentLine, currentCol);
        case ',':
            advance();
            return Token(TokenType::COMMA, ",", currentLine, currentCol);
        case ':':
            advance();
            return Token(TokenType::COLON, ":", currentLine, currentCol);
        case '(':
            advance();
            return Token(TokenType::LPAREN, "(", currentLine, currentCol);
        case ')':
            advance();
            return Token(TokenType::RPAREN, ")", currentLine, currentCol);
        case '{':
            advance();
            return Token(TokenType::LBRACE, "{", currentLine, currentCol);
        case '}':
            advance();
            return Token(TokenType::RBRACE, "}", currentLine, currentCol);
        default: {
            char ch = currentChar;
            advance();
            return Token(TokenType::ERROR, string("Unexpected character '") + ch + "'", currentLine, currentCol);
        }
    }
}

Token Lexer::peekToken() {
    size_t savedPos = position;
    size_t savedLine = line;
    size_t savedCol = column;
    char savedChar = currentChar;

    Token token = nextToken();

    position = savedPos;
    line = savedLine;
    column = savedCol;
    currentChar = savedChar;

    return token;
}

void Lexer::reset() {
    position = 0;
    line = 1;
    column = 0;
    currentChar = source.empty() ? '\0' : source[0];
}