#include <libgo/libgo.h>

#include <atomic>
#include <iostream>

// 这个文件演示：
// 1. 调度器可以启动多个线程
// 2. 协程不是只能跑在单线程里
// 3. 多线程调度更适合“高并发 I/O + 少量并行计算”的组合场景
//
// 说明：
// 纯 CPU 密集计算不会因为换成协程自动变快。
// 但当系统中既有大量 I/O 等待，又需要少量计算时，
// “少量线程 + 大量协程”的模型通常更均衡。

int main() {
    std::atomic<int> done{0};
    const int task_count = 8;

    for (int i = 0; i < task_count; ++i) {
        go [i, &done] {
            std::cout << "[task-" << i << "] 开始执行" << std::endl;
            co_sleep(30 + i * 10);
            std::cout << "[task-" << i << "] 执行完成" << std::endl;

            if (done.fetch_add(1) + 1 == task_count) {
                co_sched.Stop();
            }
        };
    }

    // 启动 4 个调度线程。
    // 在 EMS 盒子上，线程数通常不要盲目拉满，
    // 更常见做法是根据 CPU 核数、协议数量和阻塞点分布做压测确定。
    co_sched.Start(4);
    return 0;
}
