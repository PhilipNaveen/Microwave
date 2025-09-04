// Microwave Compiler - main entry point
#include "tokenizer.h"
#include "parser.h"
#include "codegen.h"
#include <iostream>
#include <fstream>
#include <sstream>

std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void writeFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (!file) {
        throw std::runtime_error("Cannot write file: " + filename);
    }
    file << content;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: microwave <source.mw> [output.c]" << std::endl;
        return 1;
    }
    
    try {
        const char* filename = argv[1];
        std::string outputFile = argc > 2 ? argv[2] : "output.c";
        
        std::cout << "Compiling " << filename << "..." << std::endl;
        
        // Read source file
        std::string source = readFile(filename);
        
        // Tokenize
        auto tokens = tokenize(source);
        std::cout << "Tokenized " << tokens.size() << " tokens." << std::endl;
        
        // Debug: Print tokens
        for (size_t i = 0; i < tokens.size() && i < 10; ++i) {
            std::cout << "Token " << i << ": " << static_cast<int>(tokens[i].type) 
                      << " '" << tokens[i].value << "'" << std::endl;
        }
        
        // Parse
        auto program = parse(tokens);
        std::cout << "Parsed " << program->functions.size() << " functions." << std::endl;
        
        // Generate C code
        std::string cCode = generateC(*program);
        
        // Write output
        writeFile(outputFile, cCode);
        std::cout << "Generated C code written to " << outputFile << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
