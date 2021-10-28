//
// Created by myralllka on 10/28/21.
//

#ifndef LU_DECOMPOSITION_BARRIER_HPP
#define LU_DECOMPOSITION_BARRIER_HPP

class barrier {
private:
    std::atomic<size_t> counter;
    std::atomic<size_t> waiting;
    std::atomic<size_t> thread_count;

    std::mutex m_m;
    std::condition_variable m_cv;

public:
    explicit barrier(size_t count) : thread_count(count), counter(0), waiting(0) {}

    void wait() {
        //fence mechanism
        ++counter;
        ++waiting;
        std::unique_lock<std::mutex> lk(m_m);
        m_cv.wait(lk, [&] { return counter >= thread_count; });
        m_cv.notify_all();
        --waiting;
        if (waiting == 0) {
            counter = 0;
        }
        lk.unlock();
    }
};

#endif //LU_DECOMPOSITION_BARRIER_HPP
