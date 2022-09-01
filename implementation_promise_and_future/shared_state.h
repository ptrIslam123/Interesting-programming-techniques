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

    class BadSharedState : public std::exception {
    public:
        explicit BadSharedState(std::string &&msg);
        virtual const char *what() const noexcept override;
    private:
        std::string msg_;
    };

    SharedState();
    ~SharedState() = default;
    bool isReady() const;
    void set(ValueType &&value);
    ValueType getIfReadyOrBlocked();

private:
    typedef util::Lazy<ValueType> Data;

    Data data_;
    std::mutex lock_;
    bool isReady_;
    std::condition_variable eventCheckerThatValueIsReady_;
};

template<typename T>
SharedState<T>::BadSharedState::BadSharedState(std::string &&msg):
msg_(std::move(msg)) {
}

template<typename T>
const char *SharedState<T>::BadSharedState::what() const noexcept {
    return msg_.c_str();
}

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
    if (isReady_) {
        throw BadSharedState("Attempt set value more than one");
    }

    data_.setValue(std::move(value));
    isReady_ = true;
    eventCheckerThatValueIsReady_.notify_one();
    uniqueLock.unlock();
}

template<typename T>
typename SharedState<T>::ValueType SharedState<T>::getIfReadyOrBlocked() {
    std::unique_lock<std::mutex> uniqueLock(lock_);
    if (!isReady_) {
        eventCheckerThatValueIsReady_.wait(uniqueLock, [this](){
            return isReady_;
        });
    }

    ValueType tmpValue;
    try {
        tmpValue = std::move(Data(std::move(data_)).getValue());
    } catch (const typename Data::BadLazy &e) {
        throw BadSharedState("Attempt bring value from future more than one");
    }

    uniqueLock.unlock();
    return std::move(tmpValue);
}

} // namespace async
