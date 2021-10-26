#include <random>
#include <algorithm>
#include <chrono>

using namespace std;
using namespace std::chrono;

class Stopwatch {
private:
    time_point<steady_clock> m_start;
    time_point<steady_clock> m_end;
    bool m_running;

public:
    Stopwatch() : m_running(false) {}

    void start() {
        m_start = steady_clock::now();
        m_running = true;
    }

    void stop() {
        m_end = steady_clock::now();
        m_running = false;
    }

    milliseconds duration() const {
        auto end = m_running ? steady_clock::now() : m_end;
        return duration_cast<milliseconds>(end - m_start);
    }
};

vector<double> generate_random_vector(int n, int seed = -1) {
    vector<double> u(n);

    random_device random_device;
    mt19937 random_engine(seed == -1 ? random_device() : seed);

    uniform_real_distribution<double> distribution(1.0, 10.0);
    generate(u.begin(), u.end(), [&random_engine, &distribution] { return distribution(random_engine); });

    return u;
}

vector<vector<double>> generate_random_matrix(int m, int n, int seed = -1) {
    random_device random_device;
    mt19937 random_engine(seed == -1 ? random_device() : seed);

    vector<vector<double>> matrix;
    for (int i = 0; i < m; i++) {
        matrix.push_back(move(generate_random_vector(n, random_engine())));
    }

    return matrix;
}

// Do not use this function in parallel vector normalization!
double compute_vector_length(const vector<double> &u) {
    double sum_squares = 0;
    for (int i = 0; i < u.size(); i++) {
        sum_squares += u[i] * u[i];
    }

    return sqrt(sum_squares);
}
