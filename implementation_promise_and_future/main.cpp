#include <iostream>
#include <chrono>
#include <mutex>
#include <sstream>
#include <thread>
#include <cassert>

#include "package_task.h"

void SleepSecond(int duration) {
    std::this_thread::sleep_for(std::chrono::milliseconds(duration * 100));
}

void PrintMessage(const std::string &msg) {
    static std::mutex displayLock;
    std::lock_guard<std::mutex> lock(displayLock);
    std::cout << msg << std::endl;
}

using namespace async;


void FirstExample() {
    auto promise = Promise<int>();
    auto future = promise.getFuture();

    std::thread otherThread([promise = std::move(promise)]() mutable {
        SleepSecond(10);
        const auto someValue = 1000;
        promise.set(someValue);
        PrintMessage("Promise is ready!");
    });
    otherThread.detach(); //! this need called before exit!

    PrintMessage("Call future get method");
    const auto result = future.get();
    std::stringstream oss;
    oss << "Result from future with value: " << result;
    PrintMessage(oss.str());
}

void SecondExample() {
    auto task = [](){
//        SleepSecond(20);
        const auto someValue = 1000;
        PrintMessage("Promise is ready!");
        return someValue;
    };
    Promise<int> promise;
    std::thread otherThread([promise = std::move(promise), task = std::move(task)]() mutable {
        promise.set(task());
    });


    PrintMessage("Call future get method");
    const auto result = promise.getFuture().get();
    std::stringstream oss;
    oss << "Result from future with value: " << result;
    PrintMessage(oss.str());
}

void ThirdExample() {
    const auto value = 1000;
    auto task = [value](){
        SleepSecond(1);
        PrintMessage("Future value is ready!");
        return value;
    };

    auto packageTask = PackageTask<int()>(task);
    auto future = packageTask.getFuture();

    SleepSecond(10);
    PrintMessage("Call future get method");
    const auto result = future.get();
    std::stringstream oss;
    oss << "Result from future with value: " << result;
    PrintMessage(oss.str());

    assert(result == value);
}

void SomeTestOnMultithreading() {
    auto globalCounter = 0;
    const auto capacity = 10000;
    std::vector<Promise<int>::Future> futures = {};
    futures.reserve(capacity);

    for (auto i = 0; i < capacity; ++i) {
        auto task = [&globalCounter](){
            return ++globalCounter;
        };
        auto packageTask = PackageTask<int()>(task);
        futures.push_back(packageTask.getFuture());
    }

    std::for_each(futures.begin(), futures.end(), [](auto &future){
        const auto result = future.get();
        std::stringstream oss;
        oss << "Result from future with value: " << result;
        PrintMessage(oss.str());
    });
    assert(globalCounter != capacity);
}

int main() {
    SomeTestOnMultithreading();
    return 0;
}
