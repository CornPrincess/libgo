# libgo C++14 中文示例

这组示例按知识点拆分，每个文件只讲一个主题，适合教学、移植和二次裁剪。

## 文件索引

- `00_overview_concepts.cpp`
  - 协程概念总览
  - 协程与线程区别
  - 使用场景
  - 解决的问题
  - 有栈/无栈协程区别
  - EMS 嵌入式盒子选型建议

- `01_basic_create_and_schedule.cpp`
  - 协程创建
  - 默认调度器

- `02_yield_and_sleep.cpp`
  - `co_yield`
  - `co_sleep`

- `03_scheduler_and_stack.cpp`
  - 自定义调度器
  - 指定协程栈大小

- `04_channel_basic.cpp`
  - 无缓冲 / 有缓冲 `Channel`

- `05_channel_timeout_and_close.cpp`
  - `TryPush`
  - `TryPop`
  - `TimedPush`
  - `TimedPop`
  - `Close`

- `06_mutex_and_rwmutex.cpp`
  - `co_mutex`
  - `co_rwmutex`

- `07_timer.cpp`
  - `co_timer`
  - `co_timer_id`

- `08_multithread_scheduler.cpp`
  - 多线程调度

- `09_cls.cpp`
  - 协程局部存储 `CLS`

- `10_defer.cpp`
  - `co_defer`
  - `co_defer_scope`

- `11_async_coroutine_pool.cpp`
  - `AsyncCoroutinePool`

- `12_connection_pool.cpp`
  - `ConnectionPool`

## 构建方法

前提：

- libgo 已安装到 `/home/libgo/local`

构建：

```bash
cmake -S /home/libgo/examples -B /home/libgo/build_examples -DLIBGO_PREFIX=/home/libgo/local
cmake --build /home/libgo/build_examples -j4
```

## 运行方法

例如运行 `Channel` 示例：

```bash
/home/libgo/build_examples/04_channel_basic
```

例如运行协程池示例：

```bash
/home/libgo/build_examples/11_async_coroutine_pool
```
