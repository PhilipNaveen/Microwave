%{
#include "microwave.tab.hpp"
#include <iostream>
#include <vector>
#include <cstdio>

using namespace std;

extern int yylex();
extern int yylineno;

void yyerror(const char *s);

class MicrowaveInterpreter {
private:
    vector<char> memory;
    int pointer;
    int reg;
    bool has_register;

public:
    MicrowaveInterpreter() : memory(30000), pointer(0), reg(0), has_register(false) {}

    void interpret(int command);
};

void yyerror(const char *s) {
    cerr << "Error: " << s << " at line " << yylineno << endl;
}

%}

%token mmm mMm mmM mMM Mmm MMm MmM MMM beep Beep beeP BEEP

%%

program:
    instructions
    ;

instructions:
    instruction instructions
    | /* empty */
    ;

instruction:
    mmm { interpreter.interpret(mmm); }
    | mMm { interpreter.interpret(mMm); }
    | mmM { interpreter.interpret(mmM); }
    | mMM { interpreter.interpret(mMM); }
    | Mmm { interpreter.interpret(Mmm); }
    | MMm { interpreter.interpret(MMm); }
    | MmM { interpreter.interpret(MmM); }
    | MMM { interpreter.interpret(MMM); }
    | beep { interpreter.interpret(beep); }
    | Beep { interpreter.interpret(Beep); }
    | beeP { interpreter.interpret(beeP); }
    | BEEP { interpreter.interpret(BEEP); }
    ;

%%

void MicrowaveInterpreter::interpret(int command) {
    switch (command) {
        case mmm:
            // This command searches backward from the current position for a matching MMM command and resumes execution from there
            {
                int nested = 0;
                int found = 0;
                for (int i = pointer - 1; i >= 0; i--) {
                    if (memory[i] == MMM) {
                        if (nested == 0) {
                            pointer = i;
                            found = 1;
                            break;
                        } else {
                            nested--;
                        }
                    } else if (memory[i] == mmm) {
                        nested++;
                    }
                }
                if (!found) {
                    cerr << "Error: Matching MMM not found for mmm" << endl;
                    exit(1);
                }
            }
            break;
        case mMm:
            // Moves current memory position back one block
            pointer = (pointer - 1 + MEMORY_SIZE) % MEMORY_SIZE;
            break;
        case mmM:
            // Moves current memory position forward one block
            pointer = (pointer + 1) % MEMORY_SIZE;
            break;
        case mMM:
            // Execute value in current memory block as if it were an instruction.
            // An invalid command exits the running program.
            // Value 3 is invalid as it would cause an infinite loop.
            switch (memory[pointer]) {
                case mmm:
                    interpret(mmm);
                    break;
                case mMm:
                    interpret(mMm);
                    break;
                case mmM:
                    interpret(mmM);
                    break;
                case Mmm:
                    interpret(Mmm);
                    break;
                case MMm:
                    interpret(MMm);
                    break;
                case MmM:
                    interpret(MmM);
                    break;
                case MMM:
                    interpret(MMM);
                    break;
                case beep:
                    interpret(beep);
                    break;
                case Beep:
                    interpret(Beep);
                    break;
                case beeP:
                    interpret(beeP);
                    break;
                case BEEP:
                    interpret(BEEP);
                    break;
                default:
                    // Invalid command, handle accordingly (e.g., exit or loop prevention)
                    cerr << "Error: Invalid command value in memory" << endl;
                    exit(1);
            }
            break;
        case Mmm:
            // If current memory block has a 0 in it, read a single ASCII character from STDIN and store it in the current memory block.
            // If the current memory block is not 0, then print the ASCII character that corresponds to the value in the current memory block to STDOUT.
            if (memory[pointer] == 0) {
                memory[pointer] = getchar();
            } else {
                putchar(memory[pointer]);
            }
            break;
        case MMm:
            // Decrement current memory block value by 1
            memory[pointer]--;
            break;
        case MmM:
            // Increment current memory block value by 1
            memory[pointer]++;
            break;
        case MMM:
            // If current memory block value is 0, skip next command and resume execution after the next matching mmm command.
            // If current memory block value is not 0, then continue with next command.
            if (memory[pointer] == 0) {
                int nested = 0;
                int found = 0;
                for (int i = pointer + 1; i < MEMORY_SIZE; i++) {
                    if (memory[i] == mmm) {
                        if (nested == 0) {
                            pointer = i;
                            found = 1;
                            break;
                        } else {
                            nested--;
                        }
                    } else if (memory[i] == MMM) {
                        nested++;
                    }
                }
                if (!found) {
                    cerr << "Error: Matching mmm not found for MMM" << endl;
                    exit(1);
                }
            }
            break;
        case beep:
            // Set current memory block value to 0
            memory[pointer] = 0;
            break;
        case Beep:
            // If register has a value, copy it to current memory block.
            // If not, store current memory block value into register.
            if (has_register) {
                memory[pointer] = reg;
                has_register = false;
            } else {
                reg = memory[pointer];
                has_register = true;
            }
            break;
        case beeP:
            // Print value of current memory block to STDOUT as an integer
            cout << static_cast<int>(memory[pointer]);
            break;
        case BEEP:
            // Read an integer from STDIN and put it into the current memory block
            cin >> memory[pointer];
            break;
        default:
            // Handle invalid command
            cerr << "Error: Invalid command" << endl;
            exit(1);
    }
}
