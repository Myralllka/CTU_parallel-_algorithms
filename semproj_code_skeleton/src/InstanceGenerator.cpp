#include <random>
#include <fstream>
#include <vector>
#include <string>

#include "Utils.h"

// Generates the records completely on random.
std::vector<std::vector<int>> random_records(std::mt19937 rand_gen, int num_records, int max_record_len) {
    std::uniform_int_distribution<int> dist_record_len(1, max_record_len);
    std::uniform_int_distribution<int> dist_values(1, 5);

    std::vector<std::vector<int>> records;
    for (auto i = 0; i < num_records; i++) {
        auto record_len = dist_record_len(rand_gen);
        std::vector<int> record;
        record.reserve(record_len);
        for (auto j = 0; j < record_len; j++) {
            record.push_back(dist_values(rand_gen));
        }
        records.push_back(record);
    }

    return records;
}

int main(int argc, char *argv[]) {
    int num_records = std::stoi(argv[1]);
    int max_record_len = std::stoi(argv[2]);
    std::string file_path = argv[3];

    std::random_device random_device;
    std::mt19937 rand_gen(random_device());

    auto records = random_records(rand_gen, num_records, max_record_len);
    write_records(records, file_path);
}
