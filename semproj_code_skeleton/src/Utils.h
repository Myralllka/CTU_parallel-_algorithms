#ifndef STORAGE_UTILS_H
#define STORAGE_UTILS_H

#include <fstream>
#include <utility>
#include <vector>
#include <string>
#include <iostream>
#include <chrono>


using namespace std::chrono;

void write_records(const std::vector<std::vector<int>> &records, const std::string &file_path) {
    std::ofstream bout(file_path.c_str(), std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);

    int num_records = records.size();
    bout.write((char *) &num_records, sizeof(int));

    for (auto &record: records) {
        int record_len = static_cast<int>(record.size());
        bout.write((char *) &record_len, sizeof(int));
        bout.write((char *) record.data(), record.size() * sizeof(int));
    }
}

std::vector<std::vector<int>> read_records(const std::string &file_path) {
    std::ifstream bin(file_path.c_str(), std::ifstream::in | std::ofstream::binary);

    int num_records = 0;
    bin.read((char *) &num_records, sizeof(int));

    std::vector<std::vector<int>> records;

    for (auto i = 0; i < num_records; i++) {
        int record_len = 0;
        bin.read((char *) &record_len, sizeof(int));

        std::vector<int> record(record_len, 0);
        bin.read((char *) record.data(), record_len * sizeof(int));

        records.push_back(record);
    }

    return records;
}

void print_records(const std::vector<std::vector<int>> &records) {
    for (auto &record: records) {
        for (auto &value: record) {
            std::cout << value << " ";
        }

        std::cout << std::endl;
    }
}

void write_cost(int cost, const std::string& file_path) {
    std::ofstream bout(file_path.c_str(), std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);

    bout.write((char *) &cost, sizeof(int));
}

class Stopwatch {
private:
    time_point<steady_clock> m_start;
    time_point<steady_clock> m_end;
    bool m_running;

public:
    Stopwatch() : m_running(false) {}

    [[maybe_unused]] void start() {
        m_start = steady_clock::now();
        m_running = true;
    }

    [[maybe_unused]] void stop() {
        m_end = steady_clock::now();
        m_running = false;
    }

    [[maybe_unused]] [[nodiscard]] milliseconds duration() const {
        auto end = m_running ? steady_clock::now() : m_end;
        return duration_cast<milliseconds>(end - m_start);
    }
};

class Program_arguments {
private:
    Program_arguments(std::string input_file_path, std::string output_file_path)
            : m_input_file_path(std::move(input_file_path)), m_output_file_path(std::move(output_file_path)) {

    }

public:
    std::string m_input_file_path;
    std::string m_output_file_path;

    static Program_arguments Parse(int argc, char *argv[]) {
        return Program_arguments{argv[1], argv[2]};
    }
};

#endif
