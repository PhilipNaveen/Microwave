%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void yyerror(const char *s);
int yylex(void);

#define MEMORY_SIZE 30000

char memory[MEMORY_SIZE];
int pointer = 0;
int reg = 0; // use 'reg' instead of 'register' to avoid conflict with C keyword
int has_register = 0;

void interpret();

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
    mmm { 
        int nested = 0;
        int found = 0;
        for (int i = yytext - 2; i >= 0; i--) {
            if (strncmp(&yytext[i], "MMM", 3) == 0) {
                if (nested == 0) {
                    yylineno = i;
                    found = 1;
                    break;
                } else {
                    nested--;
                }
            } else if (strncmp(&yytext[i], "mmm", 3) == 0) {
                nested++;
            }
        }
        if (!found) {
            fprintf(stderr, "Error: Matching MMM not found for mmm\n");
            exit(1);
        }
    }
    | mMm { pointer = (pointer - 1 + MEMORY_SIZE) % MEMORY_SIZE; }
    | mmM { pointer = (pointer + 1) % MEMORY_SIZE; }
    | mMM { interpret(); }
    | Mmm { if (memory[pointer] == 0) { memory[pointer] = getchar(); } else { putchar(memory[pointer]); } }
    | MMm { memory[pointer]--; }
    | MmM { memory[pointer]++; }
    | MMM { 
        if (memory[pointer] == 0) { 
            int nested = 0;
            int found = 0;
            for (int i = yytext - yytext; i < strlen(yytext); i++) {
                if (strncmp(&yytext[i], "mmm", 3) == 0) {
                    if (nested == 0) {
                        yylineno = i;
                        found = 1;
                        break;
                    } else {
                        nested--;
                    }
                } else if (strncmp(&yytext[i], "MMM", 3) == 0) {
                    nested++;
                }
            }
            if (!found) {
                fprintf(stderr, "Error: Matching mmm not found for MMM\n");
                exit(1);
            }
        }
    }
    | beep { memory[pointer] = 0; }
    | Beep { if (has_register) { memory[pointer] = reg; has_register = 0; } else { reg = memory[pointer]; has_register = 1; } }
    | beeP { printf("%d", memory[pointer]); }
    | BEEP { scanf("%d", &memory[pointer]); }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
}

int main(void) {
    for (int i = 0; i < MEMORY_SIZE; i++) {
        memory[i] = 0;
    }
    return yyparse();
}

void interpret() {
    switch (memory[pointer]) {
        case 1:
            pointer = (pointer - 1 + MEMORY_SIZE) % MEMORY_SIZE;
            break;
        case 2:
            pointer = (pointer + 1) % MEMORY_SIZE;
            break;
        case 3:
            // Invalid command, handle accordingly (e.g., exit or loop prevention)
            exit(1);
        case 4:
            if (memory[pointer] == 0) {
                memory[pointer] = getchar();
            } else {
                putchar(memory[pointer]);
            }
            break;
        case 5:
            memory[pointer]--;
            break;
        case 6:
            memory[pointer]++;
            break;
        case 7:
            if (memory[pointer] == 0) {
                int nested = 0;
                int found = 0;
                for (int i = yytext - yytext; i < strlen(yytext); i++) {
                    if (strncmp(&yytext[i], "mmm", 3) == 0) {
                        if (nested == 0) {
                            yylineno = i;
                            found = 1;
                            break;
                        } else {
                            nested--;
                        }
                    } else if (strncmp(&yytext[i], "MMM", 3) == 0) {
                        nested++;
                    }
                }
                if (!found) {
                    fprintf(stderr, "Error: Matching mmm not found for MMM\n");
                    exit(1);
                }
            }
            break;
        case 8:
            memory[pointer] = 0;
            break;
        case 9:
            if (has_register) {
                memory[pointer] = reg;
                has_register = 0;
            } else {
                reg = memory[pointer];
                has_register = 1;
            }
            break;
        case 10:
            printf("%d", memory[pointer]);
            break;
        case 11:
            scanf("%d", &memory[pointer]);
            break;
        default:
            // Handle invalid command
            exit(1);
    }
}
