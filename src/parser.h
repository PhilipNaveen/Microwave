#pragma once
#include "tokenizer.h"
#include <memory>
#include <string>
#include <vector>

// Forward declarations
struct Stmt;

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
struct BoolExpr : Expr {
    bool value;
    BoolExpr(bool v) : value(v) {}
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
struct UnaryExpr : Expr {
    std::string op;
    std::unique_ptr<Expr> operand;
    bool isPrefix;
    UnaryExpr(const std::string& o, std::unique_ptr<Expr> e, bool prefix = true)
        : op(o), operand(std::move(e)), isPrefix(prefix) {}
};
struct CallExpr : Expr {
    std::unique_ptr<Expr> function;
    std::vector<std::unique_ptr<Expr>> args;
    CallExpr(std::unique_ptr<Expr> f) : function(std::move(f)) {}
};
struct ArrayExpr : Expr {
    std::unique_ptr<Expr> base;
    std::unique_ptr<Expr> index;
    ArrayExpr(std::unique_ptr<Expr> b, std::unique_ptr<Expr> i)
        : base(std::move(b)), index(std::move(i)) {}
};
struct ArrayLiteralExpr : Expr {
    std::vector<std::unique_ptr<Expr>> elements;
    ArrayLiteralExpr() = default;
};

// Statements
struct Stmt : ASTNode {};
struct VarDeclStmt : Stmt {
    std::string type;
    std::string name;
    std::unique_ptr<Expr> initializer;
    VarDeclStmt(const std::string& t, const std::string& n, std::unique_ptr<Expr> init = nullptr)
        : type(t), name(n), initializer(std::move(init)) {}
};
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
struct ReturnStmt : Stmt {
    std::unique_ptr<Expr> expr;
    ReturnStmt(std::unique_ptr<Expr> e = nullptr) : expr(std::move(e)) {}
};
struct BreakStmt : Stmt {};
struct ContinueStmt : Stmt {};
struct WhileStmt : Stmt {
    std::unique_ptr<Expr> cond;
    std::vector<std::unique_ptr<Stmt>> body;
    WhileStmt(std::unique_ptr<Expr> c) : cond(std::move(c)) {}
};
struct ForStmt : Stmt {
    std::unique_ptr<Stmt> init;
    std::unique_ptr<Expr> cond;
    std::unique_ptr<Expr> update;
    std::vector<std::unique_ptr<Stmt>> body;
    ForStmt() = default;
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

// Lambda expression (defined after Stmt to avoid forward declaration issues)
struct LambdaExpr : Expr {
    std::vector<std::string> params;
    std::vector<std::unique_ptr<Stmt>> body;
    std::string returnType;
    LambdaExpr() = default;
};

// Function parameter
struct Parameter {
    std::string type;
    std::string name;
    Parameter(const std::string& t, const std::string& n) : type(t), name(n) {}
};

// Function
struct Function : ASTNode {
    std::string returnType;
    std::string name;
    std::vector<Parameter> params;
    std::vector<std::unique_ptr<Stmt>> body;
    Function(const std::string& retType, const std::string& n) : returnType(retType), name(n) {}
};

// Program
struct Program : ASTNode {
    std::vector<std::unique_ptr<Function>> functions;
};

std::unique_ptr<Program> parse(const std::vector<Token>& tokens);
