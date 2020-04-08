#pragma once
namespace tinystl {
template <typename _Tp, ::std::size_t _Nm> 
struct __array_traits {
    typedef _Tp _Type[_Nm];
    static constexpr _Tp& _S_ref(const _Type& __t, const ::std::size_t __n) {
        return const_cast<_Tp&>(__t[__n]);
    }
};

template <typename _Tp> 
struct __array_traits<_Tp, 0> {
    struct _Type {};
    static constexpr _Tp& _S_ref(const _Type& __t, const ::std::size_t __n) {
        return *static_cast<_Tp*>(nullptr);
    }
};

template <typename _Tp, ::std::size_t _Nm>
struct array {
    typedef _Tp value_type;
    typedef _Tp* pointer;
    typedef const _Tp* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef value_type* iterator;
    typedef const value_type* const_iterator;
    typedef std::size_t size_type;
    typedef __array_traits<_Tp, _Nm+1> _AT_type;
    array() = default;
    array(const value_type& __val) {
        this->fill(__val);
    }
    pointer data() {
        return &_AT_type::_S_ref(_M_elems, 0);        
    }
    const_pointer data() const {
        return &_AT_type::_S_ref(_M_elems, 0);        
    }
    const_iterator begin() const {
        return &_AT_type::_S_ref(_M_elems, 0);        
    }
    iterator begin() {
        return &_AT_type::_S_ref(_M_elems, 0);        
    }
    const_iterator end() const {
        return &_AT_type::_S_ref(_M_elems, _Nm);        
    }
    iterator end() {
        return &_AT_type::_S_ref(_M_elems, _Nm);        
    }
    reference at(const ::std::size_t __n) {
        //assert();
        return _AT_type::_S_ref(_M_elems, __n);        
    }
    const_reference at(const ::std::size_t __n) const {
        return _AT_type::_S_ref(_M_elems, __n);        
    }
    reference operator[](const ::std::size_t __n) {
        //assert();
        return _AT_type::_S_ref(_M_elems, __n);        
    }
    const_reference operator[](const ::std::size_t __n) const {
        return _AT_type::_S_ref(_M_elems, __n);        
    }
    void fill(const value_type& __val) {
        for (auto ite = begin(); ite != end(); ++ite) {
            *ite = __val;
        }
    }
private:
    typename _AT_type::_Type _M_elems;
};
}