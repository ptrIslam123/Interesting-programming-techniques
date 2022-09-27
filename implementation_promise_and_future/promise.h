#pragma once

#include <memory>
#include <functional>
#include <exception>

#include "shared_state.h"

namespace async {

template<typename T>
class Promise {
public:
    typedef T ValueType;
    typedef std::shared_ptr<SharedState<ValueType> > SharedStatePtr;
    typedef std::runtime_error BadPromise;

    class Future {
    public:
        typedef std::runtime_error BadFuture;

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

    Promise();
    Promise(Promise &&other) noexcept;
    Promise &operator=(Promise &&other) noexcept;
    Promise(const Promise &other) = delete;
    Promise &operator=(const Promise &other) = delete;
    ~Promise() = default;

    Future getFuture() const;
    void set(const ValueType &value);
    void set(ValueType &&value);
private:
    SharedStatePtr sharedState_;
};

template<typename T>
Promise<T>::Future::Future(const SharedStatePtr &sharedState):
sharedState_(sharedState) {
}

template<typename T>
Promise<T>::Future::Future(Promise::Future &&other) noexcept:
sharedState_(other.sharedState_) {
    other.sharedState_ = nullptr;
}

template<typename T>
typename Promise<T>::Future &Promise<T>::Future::operator=(Promise::Future &&other) noexcept {
    sharedState_ = other.sharedState_;
    other.sharedState_ = nullptr;
    return *this;
}

template<typename T>
bool Promise<T>::Future::isReady() const {
    return sharedState_->isReady();
}

template<typename T>
typename Promise<T>::ValueType Promise<T>::Future::get() const {
    try {
        return sharedState_->getIfReadyOrBlock();
    } catch (const typename SharedState<T>::BadSharedState &e) {
        throw BadFuture(e.what());
    }
}

template<typename T>
typename Promise<T>::Future Promise<T>::getFuture() const {
    return Future(sharedState_);
}

template<typename T>
void Promise<T>::set(const ValueType &value) {
    try {
        sharedState_->set(ValueType(value));
    } catch (const typename SharedState<T>::BadSharedState &e) {
        throw BadPromise(e.what());
    }
}

template<typename T>
void Promise<T>::set(ValueType &&value) {
    sharedState_->set(std::move(value));
}

template<typename T>
Promise<T>::Promise():
sharedState_(std::make_shared<SharedState<ValueType> >()) {
}

template<typename T>
Promise<T>::Promise(Promise &&other) noexcept:
sharedState_(other.sharedState_) {
    other.sharedState_ = nullptr;
}

template<typename T>
Promise<T> &Promise<T>::operator=(Promise &&other) noexcept {
    sharedState_ = other.sharedState_;
    other.sharedState_ = nullptr;
    return *this;
}

} // namespace async
