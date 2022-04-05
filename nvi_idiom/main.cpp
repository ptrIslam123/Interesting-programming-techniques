#include <iostream>
#include <string>

struct Base {
public:
    void foo() {
        before_foo();
        
        foo_impl();

        after_foo();
    }

    void before_foo() {
        std::cout << "Base::before_foo()" << std::endl;
    }

    void after_foo() {
        std::cout << "Base::after_foo()" << std::endl;
    }

    virtual ~Base() {}

private:
    virtual void foo_impl() = 0;
};


struct Derive1 : Base {
private:
    virtual void foo_impl() override {
        std::cout << "Derive1::foo_impl()" << std::endl;
    }
};


struct Derive2 : Base {
private:
    virtual void foo_impl() override {
        std::cout << "Derive2::foo_impl()" << std::endl;
    }
};

Base* make_instance(const std::string &instance) {
    if (instance == "Derive1") {
        return new Derive1();
    } else if (instance == "Derive2") {
        return new Derive2();
    } else {
        std::cout << "Undefine instance type: " << instance << std::endl;
        return nullptr;
    }
}


int main() {
    Base* base = make_instance("Derive1");
    std::cout << "=============== Create instance ==================" << std::endl;
    
    base->foo();
    
    delete base;
    std::cout << "================ Destroy instance =================" << std::endl;
    return 0;
}
