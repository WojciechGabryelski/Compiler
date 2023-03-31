#include <iostream>

extern void run_parser(FILE* input_file, char* output_file_name);

using namespace std;

int main(int argc, char *argv[])
{
    if (argc != 3) {
        cerr << "Usage: compiler input_file output_file\n";
        return 1;
    }
    FILE* input_file = fopen(argv[1], "r");
    if (!input_file) {
        cerr << "Error: cannot open file " << argv[1] << endl;
        return 1;
    }
    FILE* output_file = fopen(argv[2], "w");
    run_parser(input_file, argv[2]);
    fclose(input_file);
    return 0;
}
