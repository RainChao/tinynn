#pragma once
#include <typeindex>
#include <functional>

namespace tinystl 
{
//变长整形序列中的最大值
//用法: ::std::IntegerMax<1,4,3,7,2>::value 
//      output: 7
template<size_t arg, size_t... rest>
struct IntegerMax {};

template<size_t arg1, size_t arg2, size_t... rest>
struct IntegerMax<arg1, arg2, rest...> {
    static const size_t value = arg1 > arg2?
            IntegerMax<arg1, rest...>::value : IntegerMax<arg2, rest...>::value;
};

template<size_t arg>
struct IntegerMax<arg> {
    static const size_t value = arg;
};
//-----------------------------------------

//变长模版类型的最大对齐值
//用法: ::std::MaxAlign<int, char, double>::value 
//      output: 8
template<typename... Types>
struct MaxAlign : IntegerMax<::std::alignment_of<Types>::value...>
{};
//-----------------------------------------

//判断变长模版类型中是否包含某类型
//用法: 1. ::std::Contains<int, char, double>::value 
//         output: false
//      2. ::std::Contains<int, char, int>::value
//         output: true
template<typename... Types> 
struct Contains 
{};

template<typename T, typename T0, typename... Rest> 
struct Contains<T, T0, Rest...> {
    static const bool value = Contains<T, Rest...>::value;
};

template<typename T, typename... Rest> 
struct Contains<T, T, Rest...> {
    static const bool value = true;
};

template<typename T> 
struct Contains<T> {
    static const bool value = false;
};
//-----------------------------------------

//可调用类型萃取工具
//用法: 1. ::std::Contains<int, char, double>::value 
//         output: false
template<typename T>
struct Function_Traits;

template<typename R, typename... Args>
struct Function_Traits<R(Args...)> {
    static const size_t ArgNum = sizeof...(Args);
    using Ret = R;
    using FunctionType = R(Args...);
    using StlFunctionType = ::std::function<FunctionType>;
    using Pointer = R(*)(Args...);
};//common function type

template<typename R, typename... Args>
struct Function_Traits<::std::function<R(Args...)>> : Function_Traits<R(Args...)> {
};//std::function

template<typename R, typename... Args>
struct Function_Traits<R(*)(Args...)> : Function_Traits<R(Args...)> {
};//general function pointer

/*template<typename R, typename... Args>
struct Function_Traits<R(*)(Args...)> : Function_Traits<R(Args...)> {
};//class member function
*/

}
