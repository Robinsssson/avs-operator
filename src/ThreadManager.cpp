#include "ThreadManager.h"

ThreadManager::ThreadManager(std::function<void()> task) : task_(task), thread_(std::make_unique<std::thread>(task_)) {}

ThreadManager::~ThreadManager() {
    if (thread_ && thread_->joinable()) {
        thread_->join();  // 确保线程在销毁前完成
    }
}

ThreadManager::ThreadManager(ThreadManager &&other) noexcept
    : task_(std::move(other.task_)), thread_(std::move(other.thread_)) {}

ThreadManager &ThreadManager::operator=(ThreadManager &&other) noexcept {
    if (this != &other) {
        task_ = std::move(other.task_);
        thread_ = std::move(other.thread_);
    }
    return *this;
}