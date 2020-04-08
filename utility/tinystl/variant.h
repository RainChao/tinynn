#pragma once
#include <tuple>
#include <iostream>
#include <typeindex>
#include <type_traits>
#include <utility/tinystl/type_traits.h>

//实现一个类union的结构，用法同boost::variant
//使用场景：1. 多种类型的类型安全的存储和取回
//          2. 高效的、基于栈的变量存储

namespace tinystl {

template<typename... Types>
class variant {
public:
    static const size_t data_size = ::std::IntegerMax<sizeof(Types)...>::value;
    static const size_t align_size = ::std::MaxAlign<Types...>::value;
    using date_t = typename ::std::aligned_storage<data_size, align_size>::type;

    //defalut constructor
    variant() : type_index_(typeid(void)) {
    }
    //copy constructor
    variant(const variant<Types...>& other) : type_index_(typeid(void)) {
        type_index_ = other.type_index_;
        copy(type_index_, &other.buff_, &buff_);
    }
    //move constructor
    variant(variant<Types...>&& other) : type_index_(typeid(void)) {
        type_index_ = other.type_index_;
        move(type_index_, &other.buff_, &buff_);
        other.type_index_ = typeid(void);
    }
    //general constructor
    template<typename T, 
    typename = typename ::std::enable_if<::std::Contains<typename ::std::decay<T>::type, Types...>::value>::type>
    variant(T&& val) : type_index_(typeid(void)) {
        using U = typename ::std::decay<T>::type;
        type_index_ = ::std::type_index(typeid(U));
        new (&buff_) U(::std::forward<T>(val));
    }
    //destructor
    ~variant() {
        destroy(type_index_);
    }

    //copy assignment
    variant<Types...>& operator=(const variant<Types...>& other) {
        destroy(type_index_);
        type_index_ = other.type_index_;
        copy(type_index_, &other.buff_, &buff_);
        return *this;
    }
    //move assignment
    variant<Types...>& operator=(variant<Types...>&& other) {
        destroy(type_index_);
        type_index_ = other.type_index_;
        move(type_index_, &other.buff_, &buff_);
        other.type_index_ = typeid(void);
        return *this;
    }
    //general assignment
    template<typename T, 
    typename = typename ::std::enable_if<::std::Contains<typename ::std::decay<T>::type, Types...>::value>::type>
    variant<Types...>& operator=(T&& val) {
        using U = typename ::std::decay<T>::type;
        destroy(type_index_);
        type_index_ = ::std::type_index(typeid(U));
        new (&buff_) U(::std::forward<T>(val));
        return *this;
    }
    //const assignment
    template<typename T, 
    typename = typename ::std::enable_if<::std::Contains<typename ::std::decay<T>::type, Types...>::value>::type>
    variant<Types...>& operator=(const T& val) {
        using U = typename ::std::decay<T>::type;
        destroy(type_index_);
        type_index_ = ::std::type_index(typeid(U));
        new (&buff_) U(val);
        return *this;
    }

    //根据索引获取类型元函数
    template<size_t I>
    using ValueType = typename ::std::tuple_element<I, ::std::tuple<Types...>>::type;
    /*nother implementation
    template<size_t I>
    struct ValueType {
        using type = typename ::std::tuple_element<I, ::std::tuple<Types...>>::type;
    };
    */
    
    //判断是否是特定类型
    template<typename T>
    bool is() const {
        using U = typename ::std::decay<T>::type;
        return ::std::type_index(typeid(U)) == type_index_;
    }

    bool empty() const {
        return type_index_ == std::type_index(typeid(void));
    }

    //compare operator
    template<typename T, 
    typename = typename ::std::enable_if<::std::Contains<typename ::std::decay<T>::type, Types...>::value>::type>
    bool operator==(T&& val) {
        using U = typename ::std::decay<T>::type;
        if (is<U>() and
            val == *reinterpret_cast<U*>(&buff_)) {
            return true;
        }
        return false;
    }
    //get by type
    template<typename T,
             typename U = typename ::std::decay<T>::type>
    U* get() {
        if (!is<U>()) {
            return nullptr;      
        }
        return reinterpret_cast<U*>(&buff_);
    }
    //get by type const
    template<typename T,
             typename U = typename ::std::decay<T>::type>
    const U* get() const {
        if (!is<U>()) {
            return nullptr;      
        }
        return reinterpret_cast<const U*>(&buff_);
    }

private:
    void copy(const ::std::type_index& type_idx,const void* src, void* dst) {
        ::std::initializer_list<int>{(copy_<Types>(type_idx, src, dst), 0)...};
    }
    template<typename T>
    void copy_(const ::std::type_index& type_idx, const void* src, void* dst) {
       if (::std::type_index(typeid(T)) == type_idx) { 
           new (dst) T(*reinterpret_cast<const T*>(src));
       }    
    }

    void move(const ::std::type_index& type_idx, void* src, void* dst) {
        ::std::initializer_list<int>{(move_<Types>(type_idx, src, dst), 0)...};
    }
    template<typename T>
    void move_(const ::std::type_index& type_idx, void* src, void* dst) {
       if (::std::type_index(typeid(T)) == type_idx) { 
           new (dst) T(::std::move(*reinterpret_cast<T*>(src)));
       }    
    }

    void destroy(const ::std::type_index& type_idx) {
        ::std::initializer_list<int>{(destroy_<Types>(type_idx), 0)...};
    }
    template<typename T>
    void destroy_(const ::std::type_index& type_idx) {
       if (::std::type_index(typeid(T)) == type_idx) { 
           reinterpret_cast<T*>(&buff_)->~T();
       }    
    }
private:
    date_t buff_; 
    ::std::type_index type_index_;
};
}

