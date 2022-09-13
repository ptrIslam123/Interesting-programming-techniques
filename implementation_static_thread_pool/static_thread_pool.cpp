#include "static_thread_pool.h"

namespace thread {

StaticThreadPool::StaticThreadPool(const unsigned short workersCount):
tasksQueue_(),
checkerOnQueueEmpty_(),
queueLock_(),
workers_() {
    workers_.reserve(workersCount);
    for (auto i = 0; i < workersCount; ++i) {
        workers_.emplace_back([this](){
            work();
        });
    }
}

void StaticThreadPool::submit(StaticThreadPool::Task task) {
    std::lock_guard<std::mutex> lock(queueLock_);
    tasksQueue_.push(std::move(task));
    checkerOnQueueEmpty_.notify_one();
}

void StaticThreadPool::work() {
    while (true) {
        std::unique_lock<std::mutex> lock(queueLock_);
        while (tasksQueue_.empty()) {
            checkerOnQueueEmpty_.wait(lock);
        }

        auto task = tasksQueue_.front();
        tasksQueue_.pop();
        lock.unlock();

        if (!task) {
            break;
        } else {
            task();
        }
    }
}

void StaticThreadPool::join() {
    for (auto i = 0; i < workers_.size(); ++i) {
        submit({}); //! Пустой функтор сигнализирует о прекращении работы worker`а
    }

    for (auto i = 0; i < workers_.size(); ++i) {
        std::thread &worker = workers_[i];
        if (worker.joinable()) {
            worker.join();
        }
    }
}

} // namespace thread
