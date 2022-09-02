#pragma once

#include "package_task.h"

namespace async {

enum class ExecutionType {
    Async,
    Sync
};

template<typename F>
auto Async(F func, const ExecutionType executionType = ExecutionType::Sync) {
    switch (executionType) {
        case ExecutionType::Async: {
            return PackageTask<F>(func).getFuture();
        }
        case ExecutionType::Sync: {
            auto promise = Promise<std::result_of_t<F&()>>();
            promise.set(func());
            return promise.getFuture();
        }
        default: {}
    }
}

} // namespace async
