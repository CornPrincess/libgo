#include <libgo/libgo.h>

#include <functional>
#include <iostream>
#include <thread>

// 这个文件演示最基础的知识点：
// 1. 如何创建协程
// 2. 协程并不是创建后立刻执行，而是进入调度器等待调度
// 3. 单线程调度下，协程通常按进入可运行队列的顺序执行
// 4. libgo 的 go 后面可以跟函数、lambda、bind、function

void free_function() {
    std::cout << "[free_function] 我是普通函数创建的协程" << std::endl;
}

struct Worker {
    void run_by_bind() const {
        std::cout << "[Worker::run_by_bind] 我是 std::bind 创建的协程" << std::endl;
    }

    void run_by_function() const {
        std::cout << "[Worker::run_by_function] 我是 std::function 创建的协程" << std::endl;
    }
};

int main() {
    std::cout << "main: 开始投递协程，但此时还没有启动调度器" << std::endl;

    go free_function;

    go [] {
        std::cout << "[lambda] 我是 lambda 创建的协程" << std::endl;
    };

    go std::bind(&Worker::run_by_bind, Worker{});

    std::function<void()> fn = std::bind(&Worker::run_by_function, Worker{});
    go fn;

    // 这里再投递一个协程，用来安全结束调度器。
    // 注意：
    // Stop() 是“停止当前调度器”的动作，通常只在示例程序结尾使用。
    go [] {
        std::cout << "[stopper] 所有基础示例已投递，准备停止调度器" << std::endl;
        co_sched.Stop();
    };

    std::cout << "main: 调度器即将启动" << std::endl;
    co_sched.Start();
    std::cout << "main: 调度器已退出" << std::endl;
    return 0;
}
