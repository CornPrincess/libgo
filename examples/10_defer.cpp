#include <libgo/libgo.h>

#include <iostream>

// 这个文件演示：
// 1. co_defer：像 Go 一样注册一个“退出当前作用域时执行”的动作
// 2. co_defer_scope：一次写多条清理逻辑
// 3. co_last_defer().dismiss()：撤销最近一次 defer
//
// 典型用途：
// - 自动释放临时资源
// - 自动归还句柄
// - 自动打印离开日志
// - 自动回滚一段临时状态

void business_function() {
    std::cout << "[business] 进入业务函数" << std::endl;

    co_defer [] {
        std::cout << "[business] defer-1：函数退出时执行" << std::endl;
    };

    co_defer_scope {
        std::cout << "[business] defer-scope：第一条清理语句" << std::endl;
        std::cout << "[business] defer-scope：第二条清理语句" << std::endl;
    };

    co_defer [] {
        std::cout << "[business] 这条 defer 将被取消，不会执行" << std::endl;
    };
    co_last_defer().dismiss();

    std::cout << "[business] 正常业务逻辑执行中" << std::endl;
}

int main() {
    go [] {
        business_function();
    };

    go [] {
        co_sleep(50);
        co_sched.Stop();
    };

    co_sched.Start();
    return 0;
}
