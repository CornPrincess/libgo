#include <libgo/libgo.h>

#include <iostream>
#include <memory>
#include <thread>
#include <vector>

// 这个文件演示：
// 1. 默认调度器 co_sched
// 2. 自定义调度器 co::Scheduler
// 3. 指定协程栈大小 go co_stack(...)
//
// 为什么需要自定义调度器？
// 在工程里，经常希望把不同业务隔离开：
// - 采集协程在一个调度器
// - 北向上送在一个调度器
// - 本地计算在一个调度器
// 这样更便于控制资源和排障。

int main() {
    // 先创建一个额外的调度器。
    co::Scheduler* device_scheduler = co::Scheduler::Create();

    // 新调度器在后台线程中启动。
    std::thread scheduler_thread([device_scheduler] {
        device_scheduler->Start(1);
    });
    scheduler_thread.detach();

    // 在默认调度器上投递一个普通协程。
    go [] {
        std::cout << "[default scheduler] 运行在默认调度器上的协程" << std::endl;
    };

    // 在自定义调度器上投递一个协程。
    go co_scheduler(device_scheduler) [] {
        std::cout << "[custom scheduler] 运行在自定义调度器上的协程" << std::endl;
    };

    // 演示大栈协程。
    // 当协程里存在较深调用链、较大局部数组，或者接第三方老代码时，
    // 可以考虑显式指定更大的栈。
    go co_stack(256 * 1024) [] {
        std::vector<int> large_local(32 * 1024, 7);
        std::cout << "[large stack coroutine] large_local.size = "
                  << large_local.size() << std::endl;
    };

    go [] {
        co_sleep(100);
        std::cout << "[stopper] 停止默认调度器" << std::endl;
        co_sched.Stop();
    };

    co_sched.Start();
    return 0;
}
