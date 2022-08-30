#include <iostream>
#include <ucontext.h>

ucontext_t fooContext;
ucontext_t barContext;
ucontext_t mainContext;
auto callCounter = 0;

void foo() {
    //! сохраняем текущий контекс функции foo(при применении этого контекста выполнение
    //! продолжиться со следующей инструкции getcontext(&fooContext))
    getcontext(&fooContext);
    ++callCounter;
    std::cout << "Call foo with callCounter: " << callCounter << std::endl;
}

void bar() {
    //! сохраняем текущий контекс функции bar(при применении этого контекста выполнение
    //! продолжиться со следующей инструкции getcontext(&barContext))
    getcontext(&barContext);
    ++callCounter;
    std::cout << "Call bar with callCounter: " << callCounter << std::endl;
}

int main() {
    std::cout << "Init contexts" << std::endl;
    foo();
    bar();
    std::cout << "Init contexts end" << std::endl;

    //! сохраняем контекс функции main
    //! при замене текущего контекста эти через setcontext
    //! будет как будто бы мы совершили длинный переход на интсрукцию следубщую за
    //! вызововм getcontext(&mainContext), т.е. к if(callCount > 5)
    getcontext(&mainContext);
    if (callCounter > 5) {
        return 0;
    }
    //! переключаемся на контекс функции foo
    setcontext(&fooContext);
    //! переключаемся на контекс функции bar
    setcontext(&barContext);
    //! переключаемся на контекс функции main
    setcontext(&mainContext);
    return 0;
}
