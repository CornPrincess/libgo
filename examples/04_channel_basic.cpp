#include <libgo/libgo.h>

#include <iostream>
#include <memory>
#include <string>

// 这个文件演示 Channel 的基本用法：
// 1. 无缓冲 channel：发送和接收会在“时机上”互相配合
// 2. 有缓冲 channel：只要缓冲区未满，发送方可以继续推进
// 3. channel 是引用语义，拷贝 channel 仍然指向同一个底层通道

int main() {
    co_chan<int> unbuffered;

    go [unbuffered] {
        std::cout << "[producer-1] 准备向无缓冲 channel 发送 100" << std::endl;
        unbuffered << 100;
        std::cout << "[producer-1] 发送完成，说明已经有消费者接走数据" << std::endl;
    };

    go [unbuffered] {
        int value = 0;
        unbuffered >> value;
        std::cout << "[consumer-1] 从无缓冲 channel 读取到: " << value << std::endl;
    };

    co_chan<std::string> buffered(2);

    go [buffered] {
        buffered << std::string("task-A");
        buffered << std::string("task-B");
        std::cout << "[producer-2] 缓冲区容量为 2，前两个元素可以直接入队" << std::endl;
    };

    go [buffered] {
        std::string value;
        buffered >> value;
        std::cout << "[consumer-2] 读取到: " << value << std::endl;
        buffered >> value;
        std::cout << "[consumer-2] 再次读取到: " << value << std::endl;
    };

    go [] {
        co_sleep(100);
        co_sched.Stop();
    };

    co_sched.Start();
    return 0;
}
