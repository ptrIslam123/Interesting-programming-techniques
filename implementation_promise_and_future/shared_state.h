#pragma once

#include <mutex>
#include <condition_variable>
#include <atomic>
#include <exception>

#include "lazy.h"

namespace async {

template<typename T>
class SharedState {
public:
    typedef T ValueType;
    typedef std::runtime_error BadSharedState;

    SharedState();
    ~SharedState() = default;
    bool isReady() const;
    void set(ValueType &&value);
    ValueType getIfReadyOrBlock();

private:
    typedef util::Lazy<ValueType> Data;

    Data data_;
    std::mutex lock_;
    std::atomic<bool> isReady_;
    std::condition_variable eventCheckerThatValueIsReady_;
};

template<typename T>
SharedState<T>::SharedState():
data_(),
lock_(),
isReady_(false),
eventCheckerThatValueIsReady_() {
}

template<typename T>
bool SharedState<T>::isReady() const {
    return isReady_;
}

template<typename T>
void SharedState<T>::set(ValueType &&value) {
    std::unique_lock<std::mutex> uniqueLock(lock_);
    if (isReady_.load()) {
        throw BadSharedState("Attempt set value more than one");
    }

    data_.setValue(std::move(value));
    isReady_.store(true);
    eventCheckerThatValueIsReady_.notify_one();
    uniqueLock.unlock();
}

template<typename T>
typename SharedState<T>::ValueType SharedState<T>::getIfReadyOrBlock() {
    std::unique_lock<std::mutex> uniqueLock(lock_);
    if (!isReady_.load()) {
        eventCheckerThatValueIsReady_.wait(uniqueLock, [this](){
            return isReady_.load();
        });
    }

    if (data_.hasValue()) {
        auto tmpValue = std::move(Data(std::move(data_)).getValue());
        uniqueLock.unlock();
        return std::move(tmpValue);
    } else {
        uniqueLock.unlock();
        throw BadSharedState("Attempt bring value from future more than one");
    }
}

} // namespace async
