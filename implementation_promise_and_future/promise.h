#pragma once

#include <memory>
#include <functional>
#include <thread>

#include "shared_state.h"

namespace async {

template<typename T, typename ... Args>
class Promise {
public:
    typedef T ValueType;
    typedef std::shared_ptr<SharedState<ValueType> > SharedStatePtr;
    typedef std::function<T(Args ...)> AsyncTask;

    class Future {
    public:
        Future(Future &&other) noexcept;
        Future &operator=(Future &&other) noexcept;
        Future(const Future &other) = delete;
        Future &operator=(const Future &other) = delete;

        bool isReady() const;
        ValueType get() const;

    private:
        friend Promise<T>;
        explicit Future(const SharedStatePtr &sharedState);

        SharedStatePtr sharedState_;
    };


    explicit Promise(AsyncTask &&syncTask);
    ~Promise() = default;

    Future getFuture() const;
    void set(ValueType value);
private:
    SharedStatePtr sharedState_;
};

template<typename T, typename... Args>
Promise<T, Args ...>::Future::Future(const Promise::SharedStatePtr &sharedState):
sharedState_() {
}

template<typename T, typename... Args>
Promise<T, Args ...>::Future::Future(Promise::Future &&other) noexcept:
sharedState_(std::move(other.sharedState_)) {
}

template<typename T, typename... Args>
typename Promise<T, Args ...>::Future &Promise<T, Args ...>::Future::operator=(Promise::Future &&other) noexcept {
    sharedState_ = std::move(other.sharedState_);
    return *this;
}

template<typename T, typename... Args>
bool Promise<T, Args ...>::Future::isReady() const {
    return sharedState_->isReady();
}

template<typename T, typename... Args>
typename Promise<T, Args ...>::ValueType Promise<T, Args ...>::Future::get() const {
    return sharedState_->getIfReadyOrBlocked();
}

template<typename T, typename... Args>
typename Promise<T, Args ...>::Future Promise<T, Args ...>::getFuture() const {
    Future future = Future(sharedState_);
    return std::move(future);
}

template<typename T, typename... Args>
void Promise<T, Args ...>::set(ValueType value) {
    sharedState_->set(std::move(value));
}

template<typename T, typename... Args>
Promise<T, Args ...>::Promise(AsyncTask &&asyncTask):
sharedState_(std::make_shared<SharedState<T>>()) {
    std::thread thread([asyncTask, this](){
        const auto result = asyncTask();
        sharedState_->set(result);
    });
    thread.detach();
}

} // namespace async
