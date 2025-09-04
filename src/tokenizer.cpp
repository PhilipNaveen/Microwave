#include "tokenizer.h"
#include <cctype>
#include <iostream>
#include <unordered_set>

static const std::unordered_set<std::string> keywords = {
    "heat", "timer", "beep", "defrost", "mode", "popcorn", "door_closed", "door_open", 
    "if", "else", "while", "for", "break", "continue", "return", "int", "float", 
    "string", "bool", "true", "false", "lambda", "auto", "void"
};

std::vector<Token> tokenize(const std::string& source) {
    std::vector<Token> tokens;
    int line = 1, col = 1;
    size_t i = 0, n = source.size();
    
    while (i < n) {
        char c = source[i];
        
        // Skip whitespace
        if (c == ' ' || c == '\t') { 
            ++i; ++col; 
            continue; 
        }
        if (c == '\n') { 
            ++i; ++line; col = 1; 
            continue; 
        }
        
        // Identifier or keyword
        if (std::isalpha(c) || c == '_') {
            size_t start = i;
            int start_col = col;
            while (i < n && (std::isalnum(source[i]) || source[i] == '_')) {
                ++i; ++col;
            }
            std::string word = source.substr(start, i - start);
            TokenType type = keywords.count(word) ? TokenType::Keyword : TokenType::Identifier;
            tokens.push_back({type, word, line, start_col});
            continue;
        }
        
        // Number literal
        if (std::isdigit(c)) {
            size_t start = i;
            int start_col = col;
            while (i < n && std::isdigit(source[i])) {
                ++i; ++col;
            }
            // Optional decimal
            if (i < n && source[i] == '.') {
                ++i; ++col;
                while (i < n && std::isdigit(source[i])) {
                    ++i; ++col;
                }
            }
            std::string num = source.substr(start, i - start);
            tokens.push_back({TokenType::Number, num, line, start_col});
            continue;
        }
        
        // String literal
        if (c == '"') {
            size_t start = i + 1;
            int start_col = col;
            ++i; ++col;
            while (i < n && source[i] != '"') {
                if (source[i] == '\\' && i + 1 < n) {
                    i += 2; col += 2;
                } else {
                    ++i; ++col;
                }
            }
            std::string str = source.substr(start, i - start);
            if (i < n && source[i] == '"') {
                ++i; ++col;
            }
            tokens.push_back({TokenType::String, str, line, start_col});
            continue;
        }
        
        // Symbols - handle multi-character operators
        if (c == '+' || c == '-' || c == '*' || c == '/' || c == '=' || c == '<' || c == '>' || 
            c == '!' || c == '&' || c == '|' || c == '^' || c == '~' || c == '%' ||
            c == '{' || c == '}' || c == '(' || c == ')' || c == ';' || c == ',' || 
            c == '[' || c == ']' || c == '.') {
            
            int start_col = col;
            std::string op(1, c);
            ++i; ++col;
            
            // Check for multi-character operators
            if (i < n) {
                char next = source[i];
                // Two-character operators
                if ((c == '+' && next == '+') || (c == '-' && next == '-') ||
                    (c == '=' && next == '=') || (c == '!' && next == '=') ||
                    (c == '<' && next == '=') || (c == '>' && next == '=') ||
                    (c == '<' && next == '<') || (c == '>' && next == '>') ||
                    (c == '&' && next == '&') || (c == '|' && next == '|') ||
                    (c == '+' && next == '=') || (c == '-' && next == '=') ||
                    (c == '*' && next == '=') || (c == '/' && next == '=') ||
                    (c == '%' && next == '=') || (c == '^' && next == '=') ||
                    (c == '&' && next == '=') || (c == '|' && next == '=')) {
                    op += next;
                    ++i; ++col;
                    
                    // Three-character operators
                    if (i < n && op == "<<" && source[i] == '=') {
                        op += source[i];
                        ++i; ++col;
                    } else if (i < n && op == ">>" && source[i] == '=') {
                        op += source[i];
                        ++i; ++col;
                    }
                }
            }
            
            tokens.push_back({TokenType::Symbol, op, line, start_col});
            continue;
        }
        
        // Unknown character, skip
        ++i; ++col;
    }
    
    tokens.push_back({TokenType::EndOfFile, "", line, col});
    return tokens;
}
