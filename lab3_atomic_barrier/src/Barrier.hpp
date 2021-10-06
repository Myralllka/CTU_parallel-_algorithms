#include <atomic>
#include <mutex>
#include <condition_variable>

class Barrier {
private:
    std::atomic<size_t> m_waitings;
    std::atomic<size_t> m_releases;
    std::atomic_bool flag;

public:
    explicit Barrier(size_t num_threads) : m_waitings(0), m_releases(num_threads), flag(false) {}

    void wait() {
        if (m_waitings == 0) flag = false;
        ++m_waitings;

        while (!flag) {
            if (m_releases == m_waitings) {
                flag = true;
                m_waitings = 0;
                return;
            }
        }
    }
};

class BarrierCV {
private:
    std::atomic<size_t> m_waitings;
    std::atomic<size_t> m_num_of_threads;
    std::atomic_bool flag = false;
    std::mutex m_mutex{};
    std::condition_variable m_cv;

public:
    explicit BarrierCV(size_t num_threads) : m_waitings(0), m_num_of_threads(num_threads) {}

    void wait() {
        m_waitings++;
        std::unique_lock<std::mutex> lock(m_mutex);
        if (m_waitings == m_num_of_threads) {
            flag = true;
        }
        m_cv.wait(lock, [this]() { return flag == true; });
        m_cv.notify_all();
        if (m_waitings-- == 1) {
            flag = false;
        }
        lock.unlock();
    }
};