#include "codegen.h"
#include <sstream>
#include <algorithm>

                    if (leftStr && rightVar) {
                        // String + variable - use sprintf and return temp_str
                        code << "(sprintf(temp_str, \"" << leftStr->value << "%d\", " << rightVar->name << "), temp_str)";
                        return;
                    }

class CodeGenerator {
    std::stringstream code;
    int indentLevel = 0;
    int lambdaCounter = 0;
    
    void indent() {
        for (int i = 0; i < indentLevel; ++i) code << "    ";
    }
    
    std::string typeToC(const std::string& type) {
        if (type == "int") return "int";
        if (type == "float") return "float";
        if (type == "string") return "char*";
        if (type == "bool") return "int";
        if (type == "void") return "void";
        return "int"; // default to int for auto
    }
    
    void generateExpr(const Expr& expr) {
        if (auto num = dynamic_cast<const NumberExpr*>(&expr)) {
            code << num->value;
        } else if (auto str = dynamic_cast<const StringExpr*>(&expr)) {
            code << "\"" << str->value << "\"";
        } else if (auto boolean = dynamic_cast<const BoolExpr*>(&expr)) {
            code << (boolean->value ? "1" : "0");
        } else if (auto var = dynamic_cast<const VarExpr*>(&expr)) {
            // Map Microwave keywords to C equivalents
            if (var->name == "beep") {
                code << "printf";
            } else {
                code << var->name;
            }
        } else if (auto bin = dynamic_cast<const BinaryExpr*>(&expr)) {
            if (bin->op == "=") {
                generateExpr(*bin->left);
                code << " = ";
                generateExpr(*bin->right);
            } else if (bin->op == "&&") {
                generateExpr(*bin->left);
                code << " && ";
                generateExpr(*bin->right);
            } else if (bin->op == "||") {
                generateExpr(*bin->left);
                code << " || ";
                generateExpr(*bin->right);
            } else {
                // Check for string concatenation
                if (bin->op == "+") {
                    auto leftStr = dynamic_cast<const StringExpr*>(bin->left.get());
                    auto rightVar = dynamic_cast<const VarExpr*>(bin->right.get());
                    
                    if (leftStr && rightVar) {
                        // String + variable - use sprintf and return temp_str  
                        code << "(sprintf(temp_str, \"" << leftStr->value << "%d\", " << rightVar->name << "), temp_str)";
                        return;
                    }
                }
                
                code << "(";
                generateExpr(*bin->left);
                code << " " << bin->op << " ";
                generateExpr(*bin->right);
                code << ")";
            }
        } else if (auto unary = dynamic_cast<const UnaryExpr*>(&expr)) {
            if (unary->isPrefix) {
                code << unary->op;
                generateExpr(*unary->operand);
            } else {
                generateExpr(*unary->operand);
                code << unary->op;
            }
        } else if (auto call = dynamic_cast<const CallExpr*>(&expr)) {
            generateExpr(*call->function);
            code << "(";
            for (size_t i = 0; i < call->args.size(); ++i) {
                if (i > 0) code << ", ";
                generateExpr(*call->args[i]);
            }
            code << ")";
        } else if (auto lambda = dynamic_cast<const LambdaExpr*>(&expr)) {
            // Generate lambda as inline function
            (void)lambda; // Use variable to suppress warning
            std::string lambdaName = "_lambda_" + std::to_string(lambdaCounter++);
            code << lambdaName;
        } else if (auto array = dynamic_cast<const ArrayExpr*>(&expr)) {
            generateExpr(*array->base);
            code << "[";
            generateExpr(*array->index);
            code << "]";
        } else if (auto arrayLit = dynamic_cast<const ArrayLiteralExpr*>(&expr)) {
            code << "{";
            for (size_t i = 0; i < arrayLit->elements.size(); ++i) {
                if (i > 0) code << ", ";
                generateExpr(*arrayLit->elements[i]);
            }
            code << "}";
        }
    }
    
    void generateStmt(const Stmt& stmt) {
        if (auto varDecl = dynamic_cast<const VarDeclStmt*>(&stmt)) {
            indent();
            code << typeToC(varDecl->type) << " " << varDecl->name;
            if (varDecl->initializer) {
                code << " = ";
                generateExpr(*varDecl->initializer);
            }
            code << ";\n";
        } else if (auto heat = dynamic_cast<const HeatStmt*>(&stmt)) {
            indent();
            code << "heat = ";
            generateExpr(*heat->expr);
            code << ";\n";
        } else if (auto beep = dynamic_cast<const BeepStmt*>(&stmt)) {
            indent();
            code << "printf(\"%s\\n\", ";
            generateExpr(*beep->expr);
            code << ");\n";
        } else if (auto defrost = dynamic_cast<const DefrostStmt*>(&stmt)) {
            indent();
            code << defrost->varName << " = 0;\n";
        } else if (auto ret = dynamic_cast<const ReturnStmt*>(&stmt)) {
            indent();
            code << "return";
            if (ret->expr) {
                code << " ";
                generateExpr(*ret->expr);
            }
            code << ";\n";
        } else if (auto brk = dynamic_cast<const BreakStmt*>(&stmt)) {
            (void)brk; // Use variable to suppress warning
            indent();
            code << "break;\n";
        } else if (auto cont = dynamic_cast<const ContinueStmt*>(&stmt)) {
            (void)cont; // Use variable to suppress warning
            indent();
            code << "continue;\n";
        } else if (auto whileStmt = dynamic_cast<const WhileStmt*>(&stmt)) {
            indent();
            code << "while (";
            generateExpr(*whileStmt->cond);
            code << ") {\n";
            indentLevel++;
            for (const auto& s : whileStmt->body) {
                generateStmt(*s);
            }
            indentLevel--;
            indent();
            code << "}\n";
        } else if (auto forStmt = dynamic_cast<const ForStmt*>(&stmt)) {
            indent();
            code << "for (";
            if (forStmt->init) {
                // Generate init without indent and newline
                if (auto varDecl = dynamic_cast<const VarDeclStmt*>(forStmt->init.get())) {
                    code << typeToC(varDecl->type) << " " << varDecl->name;
                    if (varDecl->initializer) {
                        code << " = ";
                        generateExpr(*varDecl->initializer);
                    }
                } else if (auto exprStmt = dynamic_cast<const ExprStmt*>(forStmt->init.get())) {
                    generateExpr(*exprStmt->expr);
                }
            }
            code << "; ";
            if (forStmt->cond) {
                generateExpr(*forStmt->cond);
            }
            code << "; ";
            if (forStmt->update) {
                generateExpr(*forStmt->update);
            }
            code << ") {\n";
            indentLevel++;
            for (const auto& s : forStmt->body) {
                generateStmt(*s);
            }
            indentLevel--;
            indent();
            code << "}\n";
        } else if (auto timer = dynamic_cast<const TimerStmt*>(&stmt)) {
            indent();
            code << "for (int __i = 0; __i < ";
            generateExpr(*timer->count);
            code << "; ++__i) {\n";
            indentLevel++;
            for (const auto& s : timer->body) {
                generateStmt(*s);
            }
            indentLevel--;
            indent();
            code << "}\n";
        } else if (auto ifStmt = dynamic_cast<const IfStmt*>(&stmt)) {
            indent();
            code << "if (";
            generateExpr(*ifStmt->cond);
            code << ") {\n";
            indentLevel++;
            for (const auto& s : ifStmt->thenBody) {
                generateStmt(*s);
            }
            indentLevel--;
            indent();
            code << "}";
            if (!ifStmt->elseBody.empty()) {
                code << " else {\n";
                indentLevel++;
                for (const auto& s : ifStmt->elseBody) {
                    generateStmt(*s);
                }
                indentLevel--;
                indent();
                code << "}";
            }
            code << "\n";
        } else if (auto expr = dynamic_cast<const ExprStmt*>(&stmt)) {
            indent();
            generateExpr(*expr->expr);
            code << ";\n";
        }
    }
    
public:
    std::string generate(const Program& program) {
        code << "#include <stdio.h>\n";
        code << "#include <math.h>\n";
        code << "#include <string.h>\n\n";
        code << "char temp_str[256];\n";
        code << "int heat = 0;\n";
        code << "int door_closed = 1;\n";
        code << "int door_open = 0;\n\n";
        
        for (const auto& func : program.functions) {
            if (func->name == "main") {
                code << "int main() {\n";
            } else {
                code << typeToC(func->returnType) << " " << func->name << "(";
                for (size_t i = 0; i < func->params.size(); ++i) {
                    if (i > 0) code << ", ";
                    code << typeToC(func->params[i].type) << " " << func->params[i].name;
                }
                code << ") {\n";
            }
            
            indentLevel++;
            for (const auto& stmt : func->body) {
                generateStmt(*stmt);
            }
            indentLevel--;
            
            if (func->name == "main") {
                indent();
                code << "return 0;\n";
            }
            code << "}\n\n";
        }
        
        return code.str();
    }
};

std::string generateC(const Program& program) {
    CodeGenerator gen;
    return gen.generate(program);
}
