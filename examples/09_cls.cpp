#include <libgo/libgo.h>

#include <iostream>

// 这个文件演示 Coroutine Local Storage（CLS，协程局部存储）。
//
// 理解方式：
// - TLS 是“线程局部变量”
// - CLS 是“协程局部变量”
//
// 当多个协程复用同一个线程时，TLS 不足以隔离每个协程自己的上下文；
// 这时 CLS 就很有价值，例如：
// - 每个协程自己的 trace_id
// - 每个协程自己的请求上下文
// - 每个协程自己的统计状态

struct RequestContext {
    int request_id;
    int retry_count;

    RequestContext() : request_id(0), retry_count(0) {}
};

void process_request(const char* name, int init_id) {
    RequestContext& ctx = co_cls(RequestContext);
    ctx.request_id = init_id;

    std::cout << "[" << name << "] 初始 request_id = " << ctx.request_id
              << ", retry_count = " << ctx.retry_count << std::endl;

    ++ctx.retry_count;
    co_yield;
    ++ctx.retry_count;

    std::cout << "[" << name << "] 恢复后 request_id = " << ctx.request_id
              << ", retry_count = " << ctx.retry_count << std::endl;
}

int main() {
    go [] {
        process_request("coroutine-A", 1001);
    };

    go [] {
        process_request("coroutine-B", 2001);
    };

    go [] {
        co_sleep(50);
        co_sched.Stop();
    };

    co_sched.Start();
    return 0;
}
