#include <libgo/libgo.h>

#include <chrono>
#include <iostream>
#include <memory>

// 这个文件演示 ConnectionPool。
//
// 在 EMS/网关类项目里，很多资源都可以抽象成“连接”：
// - 数据库连接
// - 上位机会话
// - 第三方 SDK 句柄
// - 某些串口/设备对象
//
// 连接池的核心价值：
// 1. 限制最大连接数，防止把下游打垮
// 2. 复用已建立的连接，降低重复创建成本
// 3. 通过智能指针自动归还，减少遗漏

struct FakeDeviceConnection {
    explicit FakeDeviceConnection(int id) : id(id) {
        std::cout << "[factory] 创建连接 id = " << id << std::endl;
    }

    ~FakeDeviceConnection() {
        std::cout << "[deleter] 销毁连接 id = " << id << std::endl;
    }

    bool isAlive() const {
        return true;
    }

    int query_value() const {
        return id * 10;
    }

    int id;
};

bool check_alive(FakeDeviceConnection* c) {
    return c->isAlive();
}

int main() {
    int next_id = 1;

    co::ConnectionPool<FakeDeviceConnection> pool(
        [&next_id] { return new FakeDeviceConnection(next_id++); },
        nullptr,
        2,
        2
    );

    // 预热 1 条连接。
    pool.Reserve(1);

    // 普通获取。
    auto conn1 = pool.Get(&check_alive, &check_alive);
    if (conn1) {
        std::cout << "[main] conn1 query_value = " << conn1->query_value() << std::endl;
    }

    // 再获取一条，把池占满。
    auto conn2 = pool.Get();
    if (conn2) {
        std::cout << "[main] conn2 query_value = " << conn2->query_value() << std::endl;
    }

    // 第三条连接会因为达到上限而等待，这里放到协程里做超时示例。
    go [&pool] {
        auto conn3 = pool.Get(std::chrono::milliseconds(100), &check_alive, &check_alive);
        if (!conn3) {
            std::cout << "[coroutine] 100ms 内没有拿到连接，返回空指针" << std::endl;
        }

        co_sched.Stop();
    };

    co_sched.Start();

    // 离开作用域后，conn1 / conn2 会自动归还给池。
    return 0;
}
