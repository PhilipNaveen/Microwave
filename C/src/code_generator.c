#include <stdio.h>
#include <stdlib.h>

#define MEMORY_SIZE 30000

char memory[MEMORY_SIZE];
int pointer = 0;
int reg = 0; // use 'reg' instead of 'register' to avoid conflict with C keyword
int has_register = 0;

void generate_code(const char *filename);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }
    generate_code(argv[1]);
    return 0;
}

void generate_code(const char *filename) {
    FILE *input_file = fopen(filename, "r");
    if (input_file == NULL) {
        fprintf(stderr, "Error: Unable to open file %s\n", filename);
        exit(1);
    }

    FILE *output_file = fopen("output.cow", "w");
    if (output_file == NULL) {
        fprintf(stderr, "Error: Unable to create output file\n");
        exit(1);
    }

    fprintf(output_file, "#include <stdio.h>\n\nint main() {\n");
    int c;
    while ((c = fgetc(input_file)) != EOF) {
        switch (c) {
            case 'm':
                fputc('m', output_file);
                fputc('o', output_file);
                fputc('o', output_file);
                break;
            case 'M':
                fputc('M', output_file);
                fputc('O', output_file);
                fputc('O', output_file);
                break;
            case 'b':
                fputc('b', output_file);
                fputc('e', output_file);
                fputc('e', output_file);
                fputc('p', output_file);
                break;
            case 'B':
                fputc('B', output_file);
                fputc('E', output_file);
                fputc('E', output_file);
                fputc('P', output_file);
                break;
            default:
                break;
        }
        fputc(' ', output_file);
    }
    fprintf(output_file, "}\n");

    fclose(input_file);
    fclose(output_file);

    printf("Code generated successfully!\n");
}
