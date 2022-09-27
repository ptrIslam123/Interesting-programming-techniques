#pragma once

#include <functional>
#include <thread>
#include <exception>

#include "lazy.h"
#include "promise.h"

namespace async {

template<typename F>
class PackageTask {
public:
    using FunctorType = std::function<F>;
    using ReturnType = std::result_of_t<F&()>;
    using PromiseType = Promise<ReturnType>;
    using FutureType = typename PromiseType::Future;
    using BadPackageTask = std::runtime_error;

    PackageTask();
    explicit PackageTask(FunctorType functor);
    PackageTask(FunctorType functor, PromiseType &&promise);
    PackageTask(PackageTask &&other) noexcept;
    PackageTask &operator=(PackageTask &&other) noexcept;
    PackageTask(const PackageTask &other) = delete;
    PackageTask &operator=(const PackageTask &other) = delete;

    FutureType getFuture();

private:
    util::Lazy<FutureType> future_;
};

template<typename F>
PackageTask<F>::PackageTask():
future_() {
}

template<typename F>
PackageTask<F>::PackageTask(FunctorType functor):
future_() {
    *this = std::move(PackageTask(functor, PromiseType()));
}

template<typename F>
PackageTask<F>::PackageTask(FunctorType functor, PromiseType &&promise) {
    future_ = promise.getFuture();
    std::thread futureThread([promise = std::move(promise), task = std::move(functor)]() mutable {
        promise.set(task());
    });

    futureThread.detach();
}

template<typename F>
typename PackageTask<F>::FutureType PackageTask<F>::getFuture() {
    if (future_.hasValue()) {
        return std::move(util::Lazy<FutureType>(std::move(future_)).getValue());
    } else {
        throw BadPackageTask("Can`t get future from empty package task");
    }
}

template<typename F>
PackageTask<F>::PackageTask(PackageTask &&other) noexcept :
future_(std::move(other.future_)){
}

template<typename F>
PackageTask<F> &PackageTask<F>::operator=(PackageTask &&other) noexcept {
    future_ = std::move(other.future_);
    return *this;
}

} // namespace async
