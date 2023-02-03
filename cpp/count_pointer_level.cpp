#include <iostream>

struct __Dummy{};
constexpr int fuck(__Dummy*, int n){ return n; }

template <typename T>
constexpr int fuck(const T& t, int = 0){ return 0; }

    template <typename T>
constexpr int fuck(T*, int n=0)
{
    return fuck(typename std::conditional<std::is_pointer<T>::value, T, __Dummy*>::type(), n+1);
}

template <int N>
struct fuck_adapter {
    static void result(){ std::cout << "fuck you " << N << " times " << ", you are very happy!" << std::endl; }
};

template<>
struct fuck_adapter<0> {
    static void result() {  std::cout << "fuck you 0 times, you are disappointed!" << std::endl; }
};

struct You
{
    You(int a){}
};

int main(int argc, char** argv){
    constexpr You *************************************************************************
           ***********************************************************************************
           ***********************************************************************************
           *********************************************************************************** you = nullptr;
    fuck_adapter<fuck(you)>::result();

    You y(250);
    fuck_adapter<fuck(y)>::result();

    return 0;
}
