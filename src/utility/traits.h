#pragma once
#include <memory>
#include <typeinfo>
#include <cxxabi.h>
#include <iostream>
#include <typeinfo>
#include <string>
#include <sstream>
#include <type_traits>

namespace tinynn {
//is_pair_v
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

//is_char_v
template <typename T,
          typename U = ::std::decay_t<T>,
          bool v = ::std::is_same_v<U, char>> 
struct is_char {
    static constexpr bool value = v;
};
template <typename T>
inline constexpr bool is_char_v = is_char<T>::value;

//is_string_v
template <typename T,
          typename U = ::std::decay_t<T>,
          bool v = ::std::is_same_v<U, char*> or 
                   ::std::is_same_v<U, ::std::string>> 
struct is_string {
    static constexpr bool value = v;
};
template <typename T>
inline constexpr bool is_string_v = is_string<T>::value;

//is_container_v
template <typename T, typename = void>
struct is_container : ::std::false_type {};
template <typename T>
struct is_container<T, 
                    ::std::void_t<decltype(::std::declval<T>().begin()), 
                    decltype(::std::declval<T>().end()),
                    decltype(::std::declval<T>().size()), 
                    typename T::value_type>>
                    : ::std::true_type {}; 
template <typename T>
constexpr bool is_container_v = is_container<T>::value;

//is_specialization
template <typename T, template<typename...> class Ref>
struct is_specialization : ::std::false_type {};
template <typename... Args, template<typename...> class Ref>
struct is_specialization<Ref<Args...>, Ref> : ::std::true_type {};
template <typename T, template<typename...> class Ref>
constexpr bool is_specialization_v = is_specialization<T, Ref>::value;

//is_to_stream_writable_v
//https://blog.csdn.net/ding_yingzi/article/details/79983042
template<typename T, typename S = ::std::ostream, typename V = void>
struct is_to_stream_writable: std::false_type {};
template<typename T, typename S>
struct is_to_stream_writable<T, S,
           std::void_t<decltype( std::declval<S&>()<<std::declval<T>()) >>
  : std::true_type {};
template <typename T, typename S = ::std::ostream>
inline constexpr bool is_to_stream_writable_v = is_to_stream_writable<T, S>::value;

//has_member_xxxxx_v<T>进行取值
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

define_has_function(initialize);

//get_type_name_by_type<T>
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

}// tinynn