#include <random>
#include <algorithm>
#include <chrono>

class Stopwatch {
private:
    std::chrono::time_point<std::chrono::steady_clock> m_start;
    std::chrono::time_point<std::chrono::steady_clock> m_end;
    bool m_running;

public:
    Stopwatch() : m_running(false) {}

    void start() {
        m_start = std::chrono::steady_clock::now();
        m_running = true;
    }

    void stop() {
        m_end = std::chrono::steady_clock::now();
        m_running = false;
    }

    [[nodiscard]] std::chrono::milliseconds duration() const {
        auto end = m_running ? std::chrono::steady_clock::now() : m_end;
        return std::chrono::duration_cast<std::chrono::milliseconds>(end - m_start);
    }
};

std::vector<double> generate_random_vector(int n, int seed = -1) {
    std::vector<double> u(n);

    std::random_device random_device;
    std::mt19937 random_engine(seed == -1 ? random_device() : seed);

    std::uniform_real_distribution<double> distribution(1.0, 10.0);
    generate(u.begin(), u.end(), [&random_engine, &distribution] { return distribution(random_engine); });

    return u;
}

std::vector<std::vector<double>> generate_random_matrix(int m, int n, int seed = -1) {
    std::random_device random_device;
    std::mt19937 random_engine(seed == -1 ? random_device() : seed);

    std::vector<std::vector<double>> matrix;
    matrix.reserve(m);
    for (int i = 0; i < m; i++) {
        matrix.push_back(move(generate_random_vector(n, random_engine())));
    }

    return matrix;
}
