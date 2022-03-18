// #include "foo.h"

#include "main.h"

#define _BAR_SECTION_

void foo(int iter) {
    --iter;
    if (iter > 0) {
        std::cout << "Call foo: " << iter << "\n";
        bar(iter);
    }
}
