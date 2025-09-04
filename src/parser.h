#pragma once
#include "tokenizer.h"
#include <memory>
#include <string>
#include <vector>

// AST Node base
struct ASTNode {
    virtual ~ASTNode() = default;
};

// Expressions
struct Expr : ASTNode {};
struct NumberExpr : Expr {
    std::string value;
    NumberExpr(const std::string& v) : value(v) {}
};
struct StringExpr : Expr {
    std::string value;
    StringExpr(const std::string& v) : value(v) {}
};
struct VarExpr : Expr {
    std::string name;
    VarExpr(const std::string& n) : name(n) {}
};
struct BinaryExpr : Expr {
    std::string op;
    std::unique_ptr<Expr> left, right;
    BinaryExpr(const std::string& o, std::unique_ptr<Expr> l, std::unique_ptr<Expr> r)
        : op(o), left(std::move(l)), right(std::move(r)) {}
};

// Statements
struct Stmt : ASTNode {};
struct HeatStmt : Stmt {
    std::unique_ptr<Expr> expr;
    HeatStmt(std::unique_ptr<Expr> e) : expr(std::move(e)) {}
};
struct BeepStmt : Stmt {
    std::unique_ptr<Expr> expr;
    BeepStmt(std::unique_ptr<Expr> e) : expr(std::move(e)) {}
};
struct DefrostStmt : Stmt {
    std::string varName;
    DefrostStmt(const std::string& n) : varName(n) {}
};
struct TimerStmt : Stmt {
    std::unique_ptr<Expr> count;
    std::vector<std::unique_ptr<Stmt>> body;
    TimerStmt(std::unique_ptr<Expr> c) : count(std::move(c)) {}
};
struct IfStmt : Stmt {
    std::unique_ptr<Expr> cond;
    std::vector<std::unique_ptr<Stmt>> thenBody, elseBody;
    IfStmt(std::unique_ptr<Expr> c) : cond(std::move(c)) {}
};
struct ExprStmt : Stmt {
    std::unique_ptr<Expr> expr;
    ExprStmt(std::unique_ptr<Expr> e) : expr(std::move(e)) {}
};

// Function
struct Function : ASTNode {
    std::string name;
    std::vector<std::string> params;
    std::vector<std::unique_ptr<Stmt>> body;
    Function(const std::string& n) : name(n) {}
};

// Program
struct Program : ASTNode {
    std::vector<std::unique_ptr<Function>> functions;
};

std::unique_ptr<Program> parse(const std::vector<Token>& tokens);
