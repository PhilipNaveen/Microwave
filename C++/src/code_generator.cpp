#include <iostream>
#include <fstream>
#include <string>

using namespace std;

void generate_code(const string &filename);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <input_file>" << endl;
        return 1;
    }
    generate_code(argv[1]);
    return 0;
}

void generate_code(const string &filename) {
    ifstream input_file(filename);
    if (!input_file) {
        cerr << "Error: Unable to open file " << filename << endl;
        exit(1);
    }

    ofstream output_file("output.cow");
    if (!output_file) {
        cerr << "Error: Unable to create output file" << endl;
        exit(1);
    }

    char c;
    while (input_file.get(c)) {
        switch (c) {
            case 'm':
                output_file << "moo ";
                break;
            case 'M':
                output_file << "MOO ";
                break;
            case 'b':
                output_file << "beep ";
                break;
            case 'B':
                output_file << "BEEP ";
                break;
            default:
                break;
        }
    }

    cout << "Code generated successfully!" << endl;
}
