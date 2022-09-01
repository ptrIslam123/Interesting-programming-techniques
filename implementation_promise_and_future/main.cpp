#include <iostream>
#include <cassert>
#include <type_traits>
#include <chrono>
#include <mutex>
#include <sstream>
#include <functional>
#include <thread>

#include "lazy.h"
#include "promise.h"
#include "shared_state.h"

void SleepSecond(int duration) {
    std::this_thread::sleep_for(std::chrono::milliseconds(duration * 100));
}

void PrintMessage(const std::string &msg) {
    static std::mutex displayLock;
    std::lock_guard<std::mutex> lock(displayLock);
    std::cout << msg << std::endl;
}

using namespace async;

SharedState<int> sharedState;

void promise() {
    SleepSecond(10);
    PrintMessage("Promise is ready!");
    sharedState.set(1000);
}

int main() {
    std::thread promiseThread(promise);
    promiseThread.detach();

    PrintMessage("Call get value from future");
    const auto result = sharedState.getIfReadyOrBlocked();
    std::stringstream oss;
    oss << "Get value from future: " << result;
    PrintMessage(oss.str());
    return 0;
}
