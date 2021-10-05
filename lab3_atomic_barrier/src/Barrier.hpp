#include <atomic>
#include <mutex>
#include <condition_variable>

class Barrier {
private:

public:
    explicit Barrier(size_t numThreads) {}

    void wait() {
    }
};
