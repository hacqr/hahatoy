#include <iostream>

// civilized code

template <typename T>
struct PointerLevel {
    enum { count = 0 };
};

template <typename T>
struct PointerLevel<T*> {
    enum { count = PointerLevel<T>::count + 1 };
};

struct Foo
{
    Foo(int){}
};

int main(int argc, char** argv){
    Foo *************************************************************************
        ***********************************************************************************
        ***********************************************************************************
        *********************************************************************************** foo = nullptr;
    std::cout << PointerLevel<decltype(foo)>::count << std::endl;

    Foo f(520520);
    std::cout << PointerLevel<decltype(f)>::count << std::endl;

    return 0;
}
