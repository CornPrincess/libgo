#include <libgo/libgo.h>

#include <iostream>
#include <mutex>
#include <thread>

// 这个文件演示：
// 1. co_mutex：协程互斥锁
// 2. co_rwmutex：协程读写锁
// 3. 为什么不要直接拿 OS 原生 mutex 去做长时间协程等待
//
// co_mutex 的价值在于：
// 当前协程如果拿不到锁，会让出执行权，而不是把整个线程卡住。

int main() {
    co_mutex mtx;
    co_rwmutex rw;
    int shared_counter = 0;

    go [&] {
        for (int i = 0; i < 3; ++i) {
            std::lock_guard<co_mutex> lock(mtx);
            ++shared_counter;
            std::cout << "[writer-1] shared_counter = " << shared_counter << std::endl;
            co_sleep(100);
        }
    };

    go [&] {
        for (int i = 0; i < 3; ++i) {
            std::lock_guard<co_mutex> lock(mtx);
            ++shared_counter;
            std::cout << "[writer-2] shared_counter = " << shared_counter << std::endl;
            co_sleep(10);
        }
    };

    go [&] {
        {
            std::unique_lock<co_rmutex> lock(rw.Reader());
            std::cout << "[reader] 获取读锁，可与其他读者并发" << std::endl;
        }

        {
            std::unique_lock<co_wmutex> lock(rw.Writer());
            std::cout << "[writer] 获取写锁，写期间应独占" << std::endl;
        }
    };

    go [] {
        co_sleep(1200);
        co_sched.Stop();
    };

    co_sched.Start();
    return 0;
}
