#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <queue>
#include <vector>

namespace thread {

class StaticThreadPool {
public:
    typedef std::function<void()> Task;
    typedef std::queue<Task> TasksQueue;

    explicit StaticThreadPool(unsigned short workersCount);
    ~StaticThreadPool() = default;

    void submit(Task task);
    void join();

private:
    typedef std::vector<std::thread> Workers;

    void work();

    std::mutex queueLock_;
    std::condition_variable checkerOnQueueEmpty_;
    TasksQueue tasksQueue_;
    Workers workers_;
};

} // namespace thread

