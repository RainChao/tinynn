#pragma once
#include <typeinfo>
#include <cxxabi.h>
#include <global_inc.h>
namespace tinynn {

//is_pair
template <typename T>
struct is_pair : ::std::false_type {};
template <typename T> 
struct is_pair<T&> : is_pair<T> {};
template <typename T> 
struct is_pair<const T> : is_pair<T> {};
template <typename T, typename U>
struct is_pair<::std::pair<T, U>> : ::std::true_type {};
template <typename T>
inline constexpr bool is_pair_v = is_pair<T>::value;

//is_char
/*template <typename T> 
struct is_char : ::std::false_type {};
template <typename T> 
struct is_char<T&> : is_char<T> {};
template <typename T> 
struct is_char<const T> : is_char<T> {};
template <> 
struct is_char<char> : ::std::true_type {};*/
template <typename T,
          typename U = ::std::decay_t<T>,
          bool v = ::std::is_same_v<U, char>> 
struct is_char {
    static constexpr bool value = v;
};
template <typename T>
inline constexpr bool is_char_v = is_char<T>::value;

//is_string
template <typename T,
          typename U = ::std::decay_t<T>,
          bool v = ::std::is_same_v<U, char*> or 
                   ::std::is_same_v<U, ::std::string>> 
struct is_string {
    static constexpr bool value = v;
};
template <typename T>
inline constexpr bool is_string_v = is_string<T>::value;

#define define_has_function(member_function) \
    template <typename T> \
    struct has_member_##member_function { \
        template <typename U> \
        static ::std::true_type test(decltype(&U::member_function)); \
        template <typename U> \
        static ::std::false_type test(...); \
        static constexpr bool value = decltype(test<T>(0))::value; \ 
    }; \
    template <typename T> \
    inline static constexpr bool has_member_##member_function##_v = has_member_##member_function<T>::value; 

//使用has_member_xxxxx_v<T>进行取值
define_has_function(initialize);

template <typename T>
inline const ::std::string get_type_name_by_type(void) {
    int status = -1;
    ::std::string prefix_name;
    ::std::string suffix_name;
    ::std::unique_ptr<char, void(*)(void*)>  clear_name(
        abi::__cxa_demangle(typeid(T).name(), NULL, NULL, &status), ::std::free);
    ::std::string demangled_name = (status==0) ? clear_name.get() : typeid(T).name();
    if (::std::is_const<::std::remove_reference_t<T>>::value) {
        prefix_name += "const ";
    } 
    if (::std::is_volatile<::std::remove_reference_t<T>>::value) {
        prefix_name += "volatile ";
    } 
    if (::std::is_lvalue_reference<T>::value) {
        suffix_name += "&";        
    } else if (::std::is_rvalue_reference<T>::value) {
        suffix_name += "&&";
    }
    return prefix_name + demangled_name + suffix_name;
}
template <typename T>
inline const ::std::string get_type_name_by_val(T t) {
    return get_type_name_by_type<T>();
}

//has_output_stream_v 函数有问题，待验证
template <typename T>
struct has_output_stream {
    template <typename U>
    static ::std::true_type test(decltype(::std::declval<::std::ostream>() << ::std::declval<U>())*); 
    template <typename U> 
    static ::std::false_type test(...);
    static constexpr bool value = decltype(test<T>(nullptr))::value;
};
template <typename T> 
inline static constexpr bool has_output_stream_v = has_output_stream<T>::value; 

}
