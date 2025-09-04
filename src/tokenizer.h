#pragma once
#include <string>
#include <vector>

enum class TokenType {
    Keyword,
    Identifier,
    Number,
    String,
    Symbol,
    EndOfFile
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
};

std::vector<Token> tokenize(const std::string& source);
