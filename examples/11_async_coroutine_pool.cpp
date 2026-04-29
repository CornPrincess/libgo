#include <libgo/libgo.h>

#include <iostream>

// 这个文件演示协程池 AsyncCoroutinePool。
//
// 适用场景：
// 1. 你的程序主体可能是事件驱动 / 回调驱动
// 2. 但某些工作是“阻塞型”的
//    - 访问数据库
//    - 读写文件
//    - 调用阻塞 SDK
//    - 做比较耗时的同步计算
// 3. 你又不希望这些阻塞操作卡死主线程
//
// 这时可以把阻塞工作投递到协程池，由协程池在自己的调度器中处理。

int slow_calculation() {
    co_sleep(50);
    return 42;
}

void after_calculation(int& result) {
    std::cout << "[callback] 计算结果 = " << result << std::endl;
}

int main() {
    co::AsyncCoroutinePool* pool = co::AsyncCoroutinePool::Create();

    // 初始化协程池中的协程数量上限。
    pool->InitCoroutinePool(64);

    // 启动协程池：
    // minThreadNumber = 2
    // maxThreadNumber = 8
    pool->Start(2, 8);

    // 方式 1：带回调的投递。
    pool->Post<int>(&slow_calculation, &after_calculation);

    // 方式 2：用 Channel 等结果，适合协程化代码。
    co_chan<int> result_ch(1);
    pool->Post<int>(result_ch, [] {
        co_sleep(30);
        return 7 * 8;
    });

    go [result_ch] {
        int result = 0;
        result_ch >> result;
        std::cout << "[channel] 收到协程池返回值 = " << result << std::endl;
    };

    go [] {
        co_sleep(200);
        co_sched.Stop();
    };

    co_sched.Start();
    return 0;
}
