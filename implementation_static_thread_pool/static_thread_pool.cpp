#include "static_thread_pool.h"

#include <algorithm>

namespace thread {

StaticThreadPool::StaticThreadPool(const unsigned short workersCount):
tasksQueue_(),
checkerOnQueueEmpty_(),
queueLock_(),
workers_(workersCount) {
    std::generate(workers_.begin(), workers_.end(), [this](){
        return std::thread([this](){
            work();
        });
    });
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

        auto task = tasksQueue_.back();
        tasksQueue_.pop();
        lock.unlock();

        if (!task) {
            break;
        }

        task();
    }
}

void StaticThreadPool::join() {
    for (auto i = 0; i < workers_.size(); ++i) {
        submit({}); //! Пустой функтор сигнализирует о прекращении работы worker`а
    }

    std::for_each_n(workers_.begin(), workers_.size(), [](auto &worker){
        if (worker.joinable()) {
            worker.join();
        }
    });
}

} // namespace thread
