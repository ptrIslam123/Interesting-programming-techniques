// #include "bar.h"

#include "main.h"

#define _FOO_SECTION_

void bar(int iter) {
    --iter;
    if (iter > 0) {
        std::cout << "Call bar: " << iter << "\n";
        foo(iter);
    } 
}
