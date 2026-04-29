#include <libgo/libgo.h>

#include <chrono>
#include <iostream>
#include <string>

// 这个文件演示 Channel 的进阶能力：
// 1. TryPush / TryPop：非阻塞尝试
// 2. TimedPush / TimedPop：限时等待
// 3. Close：关闭通道，唤醒等待方
//
// 在嵌入式 EMS 场景里，这类能力特别有用：
// - 采集任务不能无限等待
// - 设备离线时要快速超时返回
// - 系统关闭时要通知后台协程退出

int main() {
    co_chan<int> jobs(1);

    int value = 0;
    bool ok = jobs.TryPop(value);
    std::cout << "[main] TryPop 空通道结果: " << (ok ? "成功" : "失败") << std::endl;

    ok = jobs.TryPush(10);
    std::cout << "[main] TryPush 第一次结果: " << (ok ? "成功" : "失败") << std::endl;

    ok = jobs.TryPush(20);
    std::cout << "[main] TryPush 第二次结果(缓冲已满): " << (ok ? "成功" : "失败") << std::endl;

    go [jobs] {
        int x = 0;
        if (jobs.TimedPop(x, std::chrono::milliseconds(100))) {
            std::cout << "[consumer] TimedPop 读取到: " << x << std::endl;
        }

        // 这里再尝试一次读取，因为生产者没有再送值，所以会超时。
        if (!jobs.TimedPop(x, std::chrono::milliseconds(50))) {
            std::cout << "[consumer] TimedPop 第二次超时" << std::endl;
        }
    };

    go [jobs] {
        co_sleep(80);
        std::cout << "[producer] 延迟后再投递一个值 99" << std::endl;
        bool pushed = jobs.TimedPush(99, std::chrono::milliseconds(100));
        std::cout << "[producer] TimedPush 结果: " << (pushed ? "成功" : "失败") << std::endl;

        // 关闭 channel 常用于“通知消费者没有更多数据了”。
        jobs.Close();
        std::cout << "[producer] channel 已关闭" << std::endl;
    };

    go [] {
        co_sleep(250);
        co_sched.Stop();
    };

    co_sched.Start();
    return 0;
}
