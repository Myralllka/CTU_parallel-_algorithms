#include <vector>

#include "Utils.h"

int main(int argc, char *argv[]) {
    auto program_arguments = Program_arguments::Parse(argc, argv);

    // The input records, e.g., records[0] is the first record in the input file.
    std::vector<std::vector<int>> records = read_records(program_arguments.m_input_file_path);

    // TODO: fill the tree_cost variable with the MST of the records' edit distances graph.
    int tree_cost = 0;

    std::cout << tree_cost << std::endl;
    write_cost(tree_cost, program_arguments.m_output_file_path);
}
