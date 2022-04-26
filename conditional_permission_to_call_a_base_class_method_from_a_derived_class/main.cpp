#include <iostream>
#include <cassert>

#include "image.h"




void call_class_method_example() {
    struct Test {
        Test(int id):id_(id) {}

        void foo() {
            std::cout << "id : " << id_ << std::endl;
        }

    private:
        int id_;
    };

    typedef void (Test::*Func)();
    Func f = &Test::foo;
    Test t1(10);
    Test t2(20);

    (t1.*f)();
}

int main() {

    typedef StaticImage<10> Image;

    Image image;

    assert(image.data() != NULL);

    return 0;

}
