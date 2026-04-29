#include <libgo/libgo.h>

#include <chrono>
#include <iostream>

// 这个文件演示：
// 1. co_timer：线程安全定时器
// 2. co_timer_id：撤销尚未执行的定时任务
// 3. co_sleep：适合“当前协程等待一段时间再继续”
//
// 应用场景：
// - 设备采集超时
// - 心跳超时
// - 定时上报
// - 周期性状态刷新

int main() {
    co_timer timer(std::chrono::milliseconds(1), &co_sched);

    // 先创建一个 1 秒后触发的定时器，然后立刻尝试撤销。
    co_timer_id cancelled_id = timer.ExpireAt(std::chrono::seconds(1), [] {
        std::cout << "[timer] 这条日志通常不应出现，因为前面会取消这个定时器" << std::endl;
    });

    bool cancelled = cancelled_id.StopTimer();
    std::cout << "[main] 取消定时器结果: " << (cancelled ? "成功" : "失败") << std::endl;

    timer.ExpireAt(std::chrono::milliseconds(150), [] {
        std::cout << "[timer] 150ms 定时器触发" << std::endl;
    });

    go [] {
        std::cout << "[sleep coroutine] 准备睡眠 50ms" << std::endl;
        co_sleep(50);
        std::cout << "[sleep coroutine] 睡眠结束，继续执行" << std::endl;
    };

    go [] {
        co_sleep(250);
        co_sched.Stop();
    };

    co_sched.Start();
    return 0;
}
