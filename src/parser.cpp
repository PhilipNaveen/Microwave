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
        
        // Parse return type (after mode keyword)
        std::string returnType = "void";
        if (curr().type == TokenType::Keyword && 
            (curr().value == "int" || curr().value == "float" || curr().value == "string" || 
             curr().value == "bool" || curr().value == "void")) {
            returnType = curr().value;
            advance();
        }
        
        if (curr().type != TokenType::Identifier) {
            throw std::runtime_error("Expected function name");
        }
        std::string name = curr().value;
        advance();
        
        match(TokenType::Symbol, "(");
        std::vector<Parameter> params;
        while (curr().type != TokenType::Symbol || curr().value != ")") {
            // Parse parameter type
            std::string paramType = "auto";
            if (curr().type == TokenType::Keyword && 
                (curr().value == "int" || curr().value == "float" || curr().value == "string" || 
                 curr().value == "bool" || curr().value == "auto")) {
                paramType = curr().value;
                advance();
                
                // Check for array type (int[], float[], etc.)
                if (curr().type == TokenType::Symbol && curr().value == "[") {
                    advance(); // consume '['
                    if (curr().type == TokenType::Symbol && curr().value == "]") {
                        advance(); // consume ']'
                        paramType += "[]";
                    } else {
                        throw std::runtime_error("Expected ']' after '[' in array type");
                    }
                }
            }
            
            // Parse parameter name
            if (curr().type != TokenType::Identifier) {
                throw std::runtime_error("Expected parameter name");
            }
            std::string paramName = curr().value;
            advance();
            
            params.emplace_back(paramType, paramName);
            
            if (curr().type == TokenType::Symbol && curr().value == ",") {
                advance();
            }
        }
        match(TokenType::Symbol, ")");
        match(TokenType::Symbol, "{");
        
        auto fn = std::make_unique<Function>(returnType, name);
        fn->params = params;
        while (!match(TokenType::Symbol, "}")) {
            fn->body.push_back(parseStmt());
        }
        return fn;
    }
    
    std::unique_ptr<Stmt> parseStmt() {
        std::cout << "parseStmt called, current token: " << static_cast<int>(curr().type) << " '" << curr().value << "'" << std::endl;
        // Variable declarations
        if (curr().type == TokenType::Keyword && 
            (curr().value == "int" || curr().value == "float" || curr().value == "string" || 
             curr().value == "bool" || curr().value == "auto")) {
            std::cout << "parseStmt: variable declaration branch" << std::endl;
            std::string type = curr().value;
            advance();
            
            // Check for array type (int[], float[], etc.)
            if (curr().type == TokenType::Symbol && curr().value == "[") {
                advance(); // consume '['
                if (curr().type == TokenType::Symbol && curr().value == "]") {
                    advance(); // consume ']'
                    type += "[]";
                } else {
                    throw std::runtime_error("Expected ']' after '[' in array type");
                }
            }
            
            if (curr().type != TokenType::Identifier && curr().type != TokenType::Keyword) {
                throw std::runtime_error("Expected variable name");
            }
            std::string name = curr().value;
            advance();
            
            std::unique_ptr<Expr> init = nullptr;
            if (match(TokenType::Symbol, "=")) {
                init = parseExpr();
            }
            match(TokenType::Symbol, ";");
            return std::make_unique<VarDeclStmt>(type, name, std::move(init));
        }
        
        // Control flow statements
        if (match(TokenType::Keyword, "return")) {
            std::unique_ptr<Expr> expr = nullptr;
            if (curr().type != TokenType::Symbol || curr().value != ";") {
                expr = parseExpr();
            }
            match(TokenType::Symbol, ";");
            return std::make_unique<ReturnStmt>(std::move(expr));
        }
        if (match(TokenType::Keyword, "break")) {
            match(TokenType::Symbol, ";");
            return std::make_unique<BreakStmt>();
        }
        if (match(TokenType::Keyword, "continue")) {
            match(TokenType::Symbol, ";");
            return std::make_unique<ContinueStmt>();
        }
        
        // Loop statements
        if (match(TokenType::Keyword, "while")) {
            match(TokenType::Symbol, "(");
            auto cond = parseExpr();
            match(TokenType::Symbol, ")");
            match(TokenType::Symbol, "{");
            auto stmt = std::make_unique<WhileStmt>(std::move(cond));
            while (!match(TokenType::Symbol, "}")) {
                stmt->body.push_back(parseStmt());
            }
            return stmt;
        }
        if (match(TokenType::Keyword, "for")) {
            match(TokenType::Symbol, "(");
            auto stmt = std::make_unique<ForStmt>();
            
            // Init
            if (curr().type != TokenType::Symbol || curr().value != ";") {
                stmt->init = parseStmt();
            } else {
                match(TokenType::Symbol, ";");
            }
            
            // Condition
            if (curr().type != TokenType::Symbol || curr().value != ";") {
                stmt->cond = parseExpr();
            }
            match(TokenType::Symbol, ";");
            
            // Update
            if (curr().type != TokenType::Symbol || curr().value != ")") {
                stmt->update = parseExpr();
            }
            match(TokenType::Symbol, ")");
            match(TokenType::Symbol, "{");
            
            while (!match(TokenType::Symbol, "}")) {
                stmt->body.push_back(parseStmt());
            }
            return stmt;
        }
        
        // Microwave-specific statements
        if (match(TokenType::Keyword, "heat")) {
            auto expr = parseExpr();
            match(TokenType::Symbol, ";");
            return std::make_unique<HeatStmt>(std::move(expr));
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
        std::cout << "parseStmt: expression statement branch" << std::endl;
        auto expr = parseExpr();
        match(TokenType::Symbol, ";");
        return std::make_unique<ExprStmt>(std::move(expr));
    }
    
    std::unique_ptr<Expr> parseExpr() {
        std::cout << "parseExpr called, current token: " << static_cast<int>(curr().type) 
                  << " '" << curr().value << "'" << std::endl;
        return parseAssignment();
    }
    
    std::unique_ptr<Expr> parseAssignment() {
        std::cout << "parseAssignment called, current token: " << static_cast<int>(curr().type) 
                  << " '" << curr().value << "'" << std::endl;
        auto expr = parseLogicalOr();
        
        if (curr().type == TokenType::Symbol && 
            (curr().value == "=" || curr().value == "+=" || curr().value == "-=" ||
             curr().value == "*=" || curr().value == "/=" || curr().value == "%=" ||
             curr().value == "&=" || curr().value == "|=" || curr().value == "^=" ||
             curr().value == "<<=" || curr().value == ">>=")) {
            std::string op = curr().value;
            advance();
            auto right = parseAssignment();
            return std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
        }
        
        return expr;
    }
    
    std::unique_ptr<Expr> parseLogicalOr() {
        std::cout << "parseLogicalOr called, current token: " << static_cast<int>(curr().type) << " '" << curr().value << "'" << std::endl;
        auto expr = parseLogicalAnd();
        
        while (curr().type == TokenType::Symbol && curr().value == "||") {
            std::string op = curr().value;
            advance();
            auto right = parseLogicalAnd();
            expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
        }
        
        return expr;
    }
    
    std::unique_ptr<Expr> parseLogicalAnd() {
        auto expr = parseBitwiseOr();
        
        while (curr().type == TokenType::Symbol && curr().value == "&&") {
            std::string op = curr().value;
            advance();
            auto right = parseBitwiseOr();
            expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
        }
        
        return expr;
    }
    
    std::unique_ptr<Expr> parseBitwiseOr() {
        auto expr = parseBitwiseXor();
        
        while (curr().type == TokenType::Symbol && curr().value == "|") {
            std::string op = curr().value;
            advance();
            auto right = parseBitwiseXor();
            expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
        }
        
        return expr;
    }
    
    std::unique_ptr<Expr> parseBitwiseXor() {
        auto expr = parseBitwiseAnd();
        
        while (curr().type == TokenType::Symbol && curr().value == "^") {
            std::string op = curr().value;
            advance();
            auto right = parseBitwiseAnd();
            expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
        }
        
        return expr;
    }
    
    std::unique_ptr<Expr> parseBitwiseAnd() {
        auto expr = parseEquality();
        
        while (curr().type == TokenType::Symbol && curr().value == "&") {
            std::string op = curr().value;
            advance();
            auto right = parseEquality();
            expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
        }
        
        return expr;
    }
    
    std::unique_ptr<Expr> parseEquality() {
        auto expr = parseRelational();
        
        while (curr().type == TokenType::Symbol && 
               (curr().value == "==" || curr().value == "!=")) {
            std::string op = curr().value;
            advance();
            auto right = parseRelational();
            expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
        }
        
        return expr;
    }
    
    std::unique_ptr<Expr> parseRelational() {
        auto expr = parseShift();
        
        while (curr().type == TokenType::Symbol && 
               (curr().value == "<" || curr().value == ">" || 
                curr().value == "<=" || curr().value == ">=")) {
            std::string op = curr().value;
            advance();
            auto right = parseShift();
            expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
        }
        
        return expr;
    }
    
    std::unique_ptr<Expr> parseShift() {
        auto expr = parseAdditive();
        
        while (curr().type == TokenType::Symbol && 
               (curr().value == "<<" || curr().value == ">>")) {
            std::string op = curr().value;
            advance();
            auto right = parseAdditive();
            expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
        }
        
        return expr;
    }
    
    std::unique_ptr<Expr> parseAdditive() {
        std::cout << "parseAdditive called, current token: " << static_cast<int>(curr().type) << " '" << curr().value << "'" << std::endl;
        auto expr = parseMultiplicative();
        
        while (curr().type == TokenType::Symbol && 
               (curr().value == "+" || curr().value == "-")) {
            std::cout << "parseAdditive found operator: " << curr().value << std::endl;
            std::string op = curr().value;
            advance();
            std::cout << "parseAdditive parsing right side, current token: " << static_cast<int>(curr().type) << " '" << curr().value << "'" << std::endl;
            auto right = parseMultiplicative();
            expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
        }
        
        return expr;
    }
    
    std::unique_ptr<Expr> parseMultiplicative() {
        std::cout << "parseMultiplicative called, current token: " << static_cast<int>(curr().type) << " '" << curr().value << "'" << std::endl;
        auto expr = parseUnary();
        
        while (curr().type == TokenType::Symbol && 
               (curr().value == "*" || curr().value == "/" || curr().value == "%")) {
            std::string op = curr().value;
            advance();
            auto right = parseUnary();
            expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
        }
        
        return expr;
    }
    
    std::unique_ptr<Expr> parseUnary() {
        std::cout << "parseUnary called, current token: " << static_cast<int>(curr().type) << " '" << curr().value << "'" << std::endl;
        if (curr().type == TokenType::Symbol && 
            (curr().value == "++" || curr().value == "--" || curr().value == "!" || 
             curr().value == "~" || curr().value == "+" || curr().value == "-")) {
            std::string op = curr().value;
            advance();
            auto operand = parseUnary();
            return std::make_unique<UnaryExpr>(op, std::move(operand), true);
        }
        
        return parsePostfix();
    }
    
    std::unique_ptr<Expr> parsePostfix() {
        std::cout << "parsePostfix called, current token: " << static_cast<int>(curr().type) << " '" << curr().value << "'" << std::endl;
        auto expr = parsePrimary();
        std::cout << "parsePostfix after parsePrimary, current token: " << static_cast<int>(curr().type) << " '" << curr().value << "'" << std::endl;
        
        while (true) {
            if (curr().type == TokenType::Symbol && curr().value == "(") {
                // Function call
                std::cout << "parsePostfix: function call detected" << std::endl;
                advance();
                auto call = std::make_unique<CallExpr>(std::move(expr));
                while (curr().type != TokenType::Symbol || curr().value != ")") {
                    call->args.push_back(parseExpr());
                    if (curr().type == TokenType::Symbol && curr().value == ",") {
                        advance();
                    }
                }
                match(TokenType::Symbol, ")");
                expr = std::move(call);
            } else if (curr().type == TokenType::Symbol && curr().value == "[") {
                // Array access
                std::cout << "parsePostfix: array access detected" << std::endl;
                advance();
                auto index = parseExpr();
                match(TokenType::Symbol, "]");
                expr = std::make_unique<ArrayExpr>(std::move(expr), std::move(index));
            } else if (curr().type == TokenType::Symbol && 
                       (curr().value == "++" || curr().value == "--")) {
                // Postfix increment/decrement
                std::cout << "parsePostfix: postfix operator detected" << std::endl;
                std::string op = curr().value;
                advance();
                expr = std::make_unique<UnaryExpr>(op, std::move(expr), false);
            } else {
                std::cout << "parsePostfix: breaking, current token: " << static_cast<int>(curr().type) << " '" << curr().value << "'" << std::endl;
                break;
            }
        }
        
        return expr;
    }
    
    std::unique_ptr<Expr> parsePrimary() {
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
        if (curr().type == TokenType::Keyword && 
            (curr().value == "true" || curr().value == "false")) {
            bool val = curr().value == "true";
            advance();
            return std::make_unique<BoolExpr>(val);
        }
        if (curr().type == TokenType::Symbol && curr().value == "(") {
            advance();
            auto expr = parseExpr();
            match(TokenType::Symbol, ")");
            return expr;
        }
        if (curr().type == TokenType::Symbol && curr().value == "{") {
            // Array literal
            advance();
            auto arrayLit = std::make_unique<ArrayLiteralExpr>();
            while (curr().type != TokenType::Symbol || curr().value != "}") {
                arrayLit->elements.push_back(parseExpr());
                if (curr().type == TokenType::Symbol && curr().value == ",") {
                    advance();
                }
            }
            match(TokenType::Symbol, "}");
            return arrayLit;
        }
        if (curr().type == TokenType::Keyword && curr().value == "lambda") {
            return parseLambda();
        }
        if (curr().type == TokenType::Identifier || curr().type == TokenType::Keyword) {
            std::string name = curr().value;
            advance();
            return std::make_unique<VarExpr>(name);
        }
        std::cout << "parsePrimary failed on token: " << static_cast<int>(curr().type) 
                  << " '" << curr().value << "'" << std::endl;
        throw std::runtime_error("Unexpected token in expression");
    }
    
    std::unique_ptr<LambdaExpr> parseLambda() {
        match(TokenType::Keyword, "lambda");
        match(TokenType::Symbol, "(");
        
        auto lambda = std::make_unique<LambdaExpr>();
        
        // Parse parameters
        while (curr().type != TokenType::Symbol || curr().value != ")") {
            if (curr().type == TokenType::Identifier || curr().type == TokenType::Keyword) {
                lambda->params.push_back(curr().value);
                advance();
            }
            if (curr().type == TokenType::Symbol && curr().value == ",") {
                advance();
            }
        }
        match(TokenType::Symbol, ")");
        match(TokenType::Symbol, "{");
        
        // Parse body
        while (!match(TokenType::Symbol, "}")) {
            lambda->body.push_back(parseStmt());
        }
        
        return lambda;
    }
};

std::unique_ptr<Program> parse(const std::vector<Token>& tokens) {
    Parser parser(tokens);
    return parser.parseProgram();
}
