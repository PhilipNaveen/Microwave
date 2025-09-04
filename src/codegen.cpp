#include "codegen.h"
#include <sstream>

class CodeGenerator {
    std::stringstream code;
    int indentLevel = 0;
    
    void indent() {
        for (int i = 0; i < indentLevel; ++i) code << "    ";
    }
    
    void generateExpr(const Expr& expr) {
        if (auto num = dynamic_cast<const NumberExpr*>(&expr)) {
            code << num->value;
        } else if (auto str = dynamic_cast<const StringExpr*>(&expr)) {
            code << "\"" << str->value << "\"";
        } else if (auto var = dynamic_cast<const VarExpr*>(&expr)) {
            code << var->name;
        } else if (auto bin = dynamic_cast<const BinaryExpr*>(&expr)) {
            if (bin->op == "=") {
                generateExpr(*bin->left);
                code << " = ";
                generateExpr(*bin->right);
            } else {
                generateExpr(*bin->left);
                code << " " << bin->op << " ";
                generateExpr(*bin->right);
            }
        }
    }
    
    void generateStmt(const Stmt& stmt) {
        if (auto heat = dynamic_cast<const HeatStmt*>(&stmt)) {
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
        code << "#include <stdio.h>\n\n";
        code << "int heat = 0;\n";
        code << "int door_closed = 1;\n";
        code << "int door_open = 0;\n\n";
        
        for (const auto& func : program.functions) {
            if (func->name == "main") {
                code << "int main() {\n";
            } else {
                code << "void " << func->name << "(";
                for (size_t i = 0; i < func->params.size(); ++i) {
                    if (i > 0) code << ", ";
                    code << "int " << func->params[i];
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
