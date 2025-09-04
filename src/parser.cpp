#include "parser.h"
#include <stdexcept>
#include <iostream>

class Parser {
    const std::vector<Token>& tokens;
    size_t pos = 0;
    
    Token curr() const { 
        return pos < tokens.size() ? tokens[pos] : tokens.back(); 
    }
    
    void advance() { 
        if (pos < tokens.size()) ++pos; 
    }
    
    bool match(TokenType type, const std::string& val = "") {
        if (curr().type == type && (val.empty() || curr().value == val)) {
            advance();
            return true;
        }
        return false;
    }

public:
    Parser(const std::vector<Token>& t) : tokens(t) {}
    
    std::unique_ptr<Program> parseProgram() {
        auto prog = std::make_unique<Program>();
        while (curr().type != TokenType::EndOfFile) {
            prog->functions.push_back(parseFunction());
        }
        return prog;
    }
    
    std::unique_ptr<Function> parseFunction() {
        if (!match(TokenType::Keyword, "mode")) {
            throw std::runtime_error("Expected 'mode'");
        }
        if (curr().type != TokenType::Identifier) {
            throw std::runtime_error("Expected function name");
        }
        std::string name = curr().value;
        advance();
        
        match(TokenType::Symbol, "(");
        std::vector<std::string> params;
        while (curr().type != TokenType::Symbol || curr().value != ")") {
            if (curr().type == TokenType::Identifier) {
                params.push_back(curr().value);
                advance();
            }
            if (curr().type == TokenType::Symbol && curr().value == ",") advance();
        }
        match(TokenType::Symbol, ")");
        match(TokenType::Symbol, "{");
        
        auto fn = std::make_unique<Function>(name);
        fn->params = params;
        while (!match(TokenType::Symbol, "}")) {
            fn->body.push_back(parseStmt());
        }
        return fn;
    }
    
    std::unique_ptr<Stmt> parseStmt() {
        if (match(TokenType::Keyword, "heat")) {
            auto expr = parseExpr();
            match(TokenType::Symbol, ";");
            return std::make_unique<HeatStmt>(std::move(expr));
        }
        if (match(TokenType::Keyword, "beep")) {
            match(TokenType::Symbol, "(");
            auto expr = parseExpr();
            match(TokenType::Symbol, ")");
            match(TokenType::Symbol, ";");
            return std::make_unique<BeepStmt>(std::move(expr));
        }
        if (match(TokenType::Keyword, "defrost")) {
            if (curr().type != TokenType::Identifier) {
                throw std::runtime_error("Expected variable name after defrost");
            }
            std::string var = curr().value;
            advance();
            match(TokenType::Symbol, ";");
            return std::make_unique<DefrostStmt>(var);
        }
        if (match(TokenType::Keyword, "timer")) {
            match(TokenType::Symbol, "(");
            auto count = parseExpr();
            match(TokenType::Symbol, ")");
            match(TokenType::Symbol, "{");
            auto stmt = std::make_unique<TimerStmt>(std::move(count));
            while (!match(TokenType::Symbol, "}")) {
                stmt->body.push_back(parseStmt());
            }
            return stmt;
        }
        if (match(TokenType::Keyword, "if")) {
            auto cond = parseExpr();
            match(TokenType::Symbol, "{");
            auto stmt = std::make_unique<IfStmt>(std::move(cond));
            while (!match(TokenType::Symbol, "}")) {
                stmt->thenBody.push_back(parseStmt());
            }
            if (match(TokenType::Keyword, "else")) {
                match(TokenType::Symbol, "{");
                while (!match(TokenType::Symbol, "}")) {
                    stmt->elseBody.push_back(parseStmt());
                }
            }
            return stmt;
        }
        
        // Expression statement
        auto expr = parseExpr();
        match(TokenType::Symbol, ";");
        return std::make_unique<ExprStmt>(std::move(expr));
    }
    
    std::unique_ptr<Expr> parseExpr() {
        std::cout << "Parsing expression, current token: " << static_cast<int>(curr().type) 
                  << " '" << curr().value << "'" << std::endl;
        
        if (curr().type == TokenType::Number) {
            std::string val = curr().value;
            advance();
            return std::make_unique<NumberExpr>(val);
        }
        if (curr().type == TokenType::String) {
            std::string val = curr().value;
            advance();
            return std::make_unique<StringExpr>(val);
        }
        if (curr().type == TokenType::Identifier || curr().type == TokenType::Keyword) {
            std::string name = curr().value;
            advance();
            // Binary op
            if (curr().type == TokenType::Symbol && 
                (curr().value == "+" || curr().value == "-" || 
                 curr().value == "*" || curr().value == "/" || curr().value == "=")) {
                std::string op = curr().value;
                advance();
                auto right = parseExpr();
                return std::make_unique<BinaryExpr>(op, std::make_unique<VarExpr>(name), std::move(right));
            }
            return std::make_unique<VarExpr>(name);
        }
        throw std::runtime_error("Unexpected token in expression");
    }
};

std::unique_ptr<Program> parse(const std::vector<Token>& tokens) {
    Parser parser(tokens);
    return parser.parseProgram();
}
