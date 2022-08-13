#include <iostream>
#include <thread>
#include <random>
#include <vector>
#include <mutex>
#include <future>
#include <chrono>


std::mutex stdOutLocker;

auto worker = []() {
    using namespace std::chrono_literals;
    const auto workerId = std::rand();
    
    stdOutLocker.lock();
    std::this_thread::sleep_for(1000ms);
    std::cout << "\t\t\tHello wolrd from worker id: " << workerId << std::endl;
    stdOutLocker.unlock(); 
    
    return workerId;
};

void once_thread_implement(const size_t threadCount) {
    for (auto i = 0; i < threadCount; ++i) {
        worker();
    }
}

void multi_thread_implement(const size_t threadCount) {
    using namespace std::chrono_literals;
    std::vector<std::thread> threads;
    std::vector<std::future<int>> futures;

    for (auto i = 0; i < threadCount; ++i) {
        auto task = std::packaged_task<int()>(worker);
        futures.push_back(task.get_future());
        
        std::thread thread(std::move(task));
        threads.push_back(std::move(thread));
    }

    /**
     * @brief Если неприсоединять создаваемые потоки, то при каждом вызове future[i].get() 
     * мы получим значение сразуже, как только завершиться выолнение этого потока (поток, который 
     * ассоциирован с текущим future экземпляром), даже если остальные еще выполняются. Если мы присоединим все потоки, до того
     * как мы будем запрашивать через future.get() результат выполнения, то основной (родительский) поток дождется завершения 
     * работы всех побочных потоков (которые он присоединил).
     **/
    for (auto i = 0; i < threads.size(); ++i) {
        if (threads[i].joinable()) {
            threads[i].join();
        }
    }
    // .. do something
    // std::this_thread::sleep_for(6000ms);
    for (auto i = 0; i < futures.size(); ++i) {
        std::cout << "future[" << i << "] = " << futures[i].get() << std::endl;
    }
}

int main() {


    const auto threadCount = std::thread::hardware_concurrency();
    //once_thread_implement(threadCount);
    multi_thread_implement(threadCount);
    return 0;

}
