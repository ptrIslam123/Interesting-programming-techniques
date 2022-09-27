#include <iostream>
#include <assert.h>

#include "static_thread_pool.h"


auto globalVariable = 0;

int main() {
    using namespace thread;

    auto task = [](){
        ++globalVariable;
    };

    StaticThreadPool threadPool(4);
    const auto count = 10000;
    for (auto i = 0; i < count; ++i) {
        threadPool.submit(task);
    }

    threadPool.join();

    std::cout << "globalVariable: " << globalVariable << std::endl;
    assert(globalVariable != count);
    return 0;
}
