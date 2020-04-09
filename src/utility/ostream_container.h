#pragma once
#include <map>
#include <set>
#include <list>
#include <array>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <memory>
#include <typeinfo>
#include <vector>
#include <string>
#include <functional>
#include <type_traits>
#include <unordered_map>  
#include <unordered_set>  
#include <utility/traits.h>
#include <initializer_list>

//learn from: https://github.com/p-ranav/pprint
namespace tinynn {
class PrettyPrinter {
public:
    PrettyPrinter(::std::ostream& ostream) : _ostream(ostream) {}

    template <typename T>
    void print(const T& val) {
        print_internal(val, 0, "");
        print_internal_without_quotas("", 0, _line_split);        
    }

    template <typename T, typename... Tothers>
    void print(const T& val, const Tothers&... vals) {
        print_internal(val, 0, "");
        bool current_quotas = _quotas;
        _quotas = false;
        print_internal(" ", 0, "");
        _quotas = current_quotas;
        print(vals...);
    }
private:
    //for float
    void print_internal(const float str, 
                        const int indent = 0, 
                        const ::std::string& line_split = "\n") {
        _ostream << ::std::string(indent, ' ') << str << "f" << line_split;
    } 

    //for double
    void print_internal(const double str, 
                        const int indent = 0, 
                        const ::std::string& line_split = "\n") {
        _ostream << ::std::string(indent, ' ') << str << line_split;
    } 

    //for char
    void print_internal(const char& str, 
                        const int indent = 0, 
                        const ::std::string& line_split = "\n") {
        _ostream << ::std::string(indent, ' ') 
                 << (_quotas? "\'":"") << str << (_quotas? "\'":"") << line_split;
    } 

    template <typename ...>
    void print_internal(...);

    //for integral
    template <typename T> 
    auto print_internal(const T& val, const int indent = 0, 
                        const ::std::string& line_split = "\n") 
    -> ::std::enable_if_t<::std::is_integral_v<T>, void> {
        _ostream << std::string(indent, ' ') << val << line_split;
    }

    //for string
    template <typename T> 
    auto print_internal(const T& str, 
                        const int indent = 0, 
                        const ::std::string& line_split = "\n")
    -> ::std::enable_if_t<is_string_v<T>, void> {
        _ostream << ::std::string(indent, ' ') 
                 << (_quotas? "\"":"") << str << (_quotas? "\"":"") << line_split;
    } 
    template <typename T> 
    auto print_internal_without_quotas(const T& str, 
                        const int indent = 0, 
                        const ::std::string& line_split = "\n")
    -> ::std::enable_if_t<is_string_v<T>, void> {
        _ostream << ::std::string(indent, ' ') << str << line_split;
    } 

    //for nullptr
    template <typename T> 
    auto print_internal(const T& val, const int indent = 0, 
                        const ::std::string& line_split = "\n") 
    -> ::std::enable_if_t<::std::is_null_pointer_v<T>, void> {
        _ostream << std::string(indent, ' ') << "nullptr" << line_split;
    }

    //for pointer
    template <typename T> 
    auto print_internal(const T& val, const int indent = 0, 
                        const ::std::string& line_split = "\n") 
    -> ::std::enable_if_t<::std::is_pointer_v<T> or 
                          is_specialization_v<T, ::std::shared_ptr> or
                          is_specialization_v<T, ::std::unique_ptr>, void> {
        if (val == nullptr) {
            print_internal(nullptr, indent, line_split);
        } else {
            _ostream << std::string(indent, ' ') << "<" << get_type_name_by_type<T>() 
                     << " at " << val << ">" << line_split;
        }
    }
    
    //for ::std::pair
    template <typename F, typename S> 
    void print_internal(const ::std::pair<F,S>& val, const int indent = 0,
                        const ::std::string& line_split = "\n") {
        print_internal(val.first, indent, line_split);
        print_internal_without_quotas(" : ", 0 , ""); 
        print_internal(val.second, 0, line_split); 
    }

    //for class which is writable
    template <typename T> 
    auto print_internal(const T& val, const int indent = 0, 
                        const ::std::string& line_split = "\n") 
    -> ::std::enable_if_t<::std::is_class_v<T> and 
                          is_to_stream_writable_v<T> and
                          !is_string_v<T> and 
                          !is_specialization_v<T, ::std::shared_ptr> and
                          !is_specialization_v<T, ::std::unique_ptr>, void> {
        _ostream << std::string(indent, ' ') << val << line_split;
    }

    //for class which is not writable
    template <typename T> 
    auto print_internal(const T& val, const int indent = 0, 
                        const ::std::string& line_split = "\n") 
    -> ::std::enable_if_t<::std::is_class_v<T> and 
                          !is_to_stream_writable_v<T> and
                          !is_container_v<T> and
                          !is_pair_v<T>, void> {
        _ostream << std::string(indent, ' ') << "<Object " 
                 << get_type_name_by_type<T>() << ">" << line_split;
    }

    //for container which is iterable and linable
    template <typename T>
    auto print_internal(const T& container, const int indent = 0, 
                        const ::std::string& line_split = "\n") 
    -> ::std::enable_if_t<is_specialization_v<T, ::std::vector> or
                          is_specialization_v<T, ::std::list>, void> {
        using ValueType = typename T::value_type;
        if (container.size() == 0) {
            print_internal_without_quotas("[]", indent, "");
        } else if (container.size() == 1) {
            print_internal_without_quotas("[", indent, "");
            for (auto& item : container) {
                print_internal(item, 0, "");
            }
            print_internal_without_quotas("]", 0, "");
        } else {
            if (is_container_v<ValueType> and !is_string_v<ValueType>) {
                print_internal_without_quotas("[", indent, _line_split);
                auto ite = container.begin();
                print_internal(*ite, indent + _indent, _line_split);
                for (++ite; ite != container.end(); ++ite) { 
                    print_internal_without_quotas(",\n", 0, "");
                    print_internal(*ite, indent + _indent, _line_split);
                }
                print_internal_without_quotas("\n", 0, "");
                print_internal_without_quotas("]", indent, "");
            }  else {
                print_internal_without_quotas("[", indent, "");
                auto ite = container.begin();
                print_internal(*ite, 0, "");
                for (++ite; ite != container.end(); ++ite) { 
                    print_internal_without_quotas(", ", 0, "");
                    print_internal(*ite, 0, "");
                }
                print_internal_without_quotas("]", 0, "");
            }
        }
    }
    ///for container which is dict
    template <typename T>
    auto print_internal(const T& container, const int indent = 0, 
                        const ::std::string& line_split = "\n") 
    -> ::std::enable_if_t<is_container_v<T> and 
                          !is_string_v<T> and
                          (is_specialization_v<T, ::std::set> or
                          is_specialization_v<T, ::std::map> or
                          is_specialization_v<T, ::std::unordered_map> or
                          is_specialization_v<T, ::std::unordered_set>), void> {
        using ValueType = typename T::value_type;
        if (container.size() == 0) {
            print_internal_without_quotas("{}", indent, "");
        } else if (container.size() == 1) {
            print_internal_without_quotas("{", indent, "");
            for (auto& item : container) {
                print_internal(item, 0, "");
            }
            print_internal_without_quotas("}", 0, "");
        } else {
            if (is_container_v<ValueType> and !is_string_v<ValueType>) {
                print_internal_without_quotas("{", indent, _line_split);
                auto ite = container.begin();
                print_internal(*ite, indent + _indent, _line_split);
                for (++ite; ite != container.end(); ++ite) { 
                    print_internal_without_quotas(",\n", 0, "");
                    print_internal(*ite, indent + _indent, _line_split);
                }
                print_internal_without_quotas("\n", 0, "");
                print_internal_without_quotas("}", indent, "");
            }  else {
                print_internal_without_quotas("{", indent, "");
                auto ite = container.begin();
                print_internal(*ite, 0, "");
                for (++ite; ite != container.end(); ++ite) { 
                    print_internal_without_quotas(", ", 0, "");
                    print_internal(*ite, 0, "");
                }
                print_internal_without_quotas("}", 0, "");
            }
        }
    }
private:
    bool _quotas = true;
    ::std::ostream& _ostream;
    ::std::string _line_split = "\n";
    ::size_t _indent = 3;
};

} //tinynn
