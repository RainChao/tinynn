#pragma once
#include <utility/traits.h>
namespace tinynn {

template <typename T>
void output(const bool is_char,
            const bool is_string,
            ::std::ostream& os,
            const T& val) {
    if (is_char) {
        os << "\'" << val << "\'";
    } else if (is_string) {
        os << "\"" << val << "\"";
    } else {
        os << val;
    }
}
template <typename T, 
          ::std::enable_if_t<is_pair_v<T>, bool> = true> 
::std::ostream& out_element(::std::ostream& os,const T& element) {
    os << "(";
    constexpr bool is_char = is_char_v<decltype(element.first)>;
    constexpr bool is_string = is_string_v<decltype(element.first)>;
    output(is_char, is_string, os, element.first);
    os << " -> ";
    constexpr bool is_char_s = is_char_v<decltype(element.second)>;
    constexpr bool is_string_s = is_string_v<decltype(element.second)>;
    output(is_char_s, is_string_s, os, element.second);
    os << ")";
}
template <typename T, 
          ::std::enable_if_t<!is_pair_v<T>, bool> = true> 
::std::ostream& out_element(::std::ostream& os,const T& element) {
    constexpr bool is_char = is_char_v<T>;
    constexpr bool is_string = is_string_v<T>;
    output(is_char, is_string, os, element);
    return os;
}
//only support container
template <typename T> 
auto out_container(::std::ostream& os, T& container) 
  -> decltype(container.begin(), container.end(), os) {
    os << "{";
    bool first_ele = true;
    for (auto& ele : container) {
        if (!first_ele) { os << ", "; }
        out_element(os, ele);
        first_ele = false;
    }
    os << "}";
}

template <typename T, typename U>
::std::ostream& operator<< (::std::ostream& os,const ::std::pair<T, U>& val) {
    return out_element(os, val);
}
template <typename T>
::std::ostream& operator<< (::std::ostream& os,const ::std::vector<T>& val) {
    return out_container(os, val);
}
template <typename T>
::std::ostream& operator<< (::std::ostream& os,const ::std::set<T>& val) {
    return out_container(os, val);
}
template <typename T, typename U>
::std::ostream& operator<< (::std::ostream& os,const ::std::map<T, U>& val) {
    return out_container(os, val);
}

} //tinynn