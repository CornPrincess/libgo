#include <libgo/libgo.h>

#include <iostream>
#include <thread>

// 这个文件演示：
// 1. co_yield：主动让出执行权
// 2. co_sleep：只挂起当前协程，不阻塞整个调度线程
// 3. 协程的切换是“协作式”的，任务自己在合适的点让出执行权

int main() {
    go [] {
        std::cout << "[A] 第 1 步" << std::endl;
        co_yield;
        std::cout << "[A] 第 2 步" << std::endl;
        co_sleep(50);
        std::cout << "[A] 第 3 步，睡眠后恢复执行" << std::endl;
    };

    go [] {
        std::cout << "[B] 第 1 步" << std::endl;
        co_yield;
        std::cout << "[B] 第 2 步" << std::endl;
        co_sleep(10);
        std::cout << "[B] 第 3 步，短睡眠更早恢复" << std::endl;
    };

    go [] {
        // 这个协程只是负责在足够晚的时刻结束调度器。
        // 如果结束太早，前面的休眠协程还没恢复，程序就会直接退出。
        co_sleep(120);
        std::cout << "[stopper] 示例结束，停止调度器" << std::endl;
        co_sched.Stop();
    };

    co_sched.Start();
    return 0;
}
