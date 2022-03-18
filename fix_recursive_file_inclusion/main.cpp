// #include "foo.h"
// #include "bar.h"

#include "main.h"

#define _FOO_SECTION_
#define _BAR_SECTION_

int main() {
    const int iteration_counter = 10;
    foo(iteration_counter);
    bar(iteration_counter);
    return 0;
}
