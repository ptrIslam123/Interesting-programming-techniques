#include <iostream>
#include <chrono>
#include <mutex>
#include <sstream>
#include <thread>

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


void firstExample() {
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

void secondExample() {
    auto task = [](){
        SleepSecond(10);
        const auto someValue = 1000;
        PrintMessage("Promise is ready!");
        return someValue;
    };

    PackageTask<int()> packageTask(task);
    PrintMessage("Call future get method");
    const auto result = packageTask.getFuture().get();
    std::stringstream oss;
    oss << "Result from future with value: " << result;
    PrintMessage(oss.str());
}

int main() {
    secondExample();
    return 0;
}
