#ifndef __BASETHREAD__H__
#define __BASETHREAD__H__
#include <functional>
#include <memory>
#include <thread>
class ThreadManager {
   public:
    // 构造函数：启动线程
    ThreadManager(std::function<void()> task);

    // 析构函数：等待线程结束
    ~ThreadManager();

    // 防止拷贝构造
    ThreadManager(const ThreadManager &) = delete;

    // 防止拷贝赋值
    ThreadManager &operator=(const ThreadManager &) = delete;

    // 移动构造
    ThreadManager(ThreadManager &&other) noexcept;

    // 移动赋值
    ThreadManager &operator=(ThreadManager &&other) noexcept;

   private:
    std::function<void()> task_;           // 存储任务
    std::unique_ptr<std::thread> thread_;  // 使用智能指针管理线程
};

#endif  //!__BASETHREAD__H__
