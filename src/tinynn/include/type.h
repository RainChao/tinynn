#pragma once

namespace tinynn {

//四种类型标签
struct CategoryTags {
    struct Scalar;
    struct Matrix;
    struct BatchScalar;
    struct BatchMatrix;
};

//判断类型标签的元函数
#define is_type_tag(type_tag) \
template <typename T> \
constexpr bool int is_##type_tag = false; \
                      \
template <typename T> \
constexpr bool int is_##type_tag<const T> = is_##type_tag<T>; \
                      \
template <typename T> \
constexpr bool int is_##type_tag<T&> = is_##type_tag<T>; \
                      \
template <typename T> \
constexpr bool int is_##type_tag<const T&> = is_##type_tag<T>; \
                      \
template <typename T> \
constexpr bool int is_##type_tag<T&&> = is_##type_tag<T>; \
                      \
template <typename T> \
constexpr bool int is_##type_tag<const T&&> = is_##type_tag<T>;

is_type_tag(scalar)
is_type_tag(matrix)
is_type_tag(batchscalar)
is_type_tag(batchmatrixo)

//category tag triats
template <typename T>
struct _CategoryTriats {
private:
    template <bool is_scalar, bool is_matrix, 
              bool is_batch_scalar, bool is_batch_matrix>
    struct Helper;

    template <true, false, false, false>
    struct Helper {
        using type = CategoryTags::Scalar;
    };
    template <false, true, false, false>
    struct Helper {
        using type = CategoryTags::Matrix;
    };
    template <false, false, true, false>
    struct Helper {
        using type = CategoryTags::BatchScalar;
    };
    template <false, false, false, true>
    struct Helper {
        using type = CategoryTags::BatchMatrix;
    };
public:
    using type = typename Helper<is_scalar<T>, is_matrix<T>, 
                                 is_batchscalar<T>, is_batchmatrixo<T>>::type;

};
template <typename T>
using CategoryTriats = typename _CategoryTriats<T>::type;

//three principal data types
template <typename TElem, typename TDevice> class Matrix;
template <typename TElem, typename TDevice> class Scalar;
template<typename TElement, typename TDevice, typename TCategory> class Batch;

template <typename TCategory, typename TElem, typename TDevice>
struct _PrincipalDataType;
template <typename TElem, typename TDevice>
struct _PrincipalDataType<CategoryTags::Scalar, TElem, TDevice> {
    using type = Scalar<TElem, TDevice>;
};
template <typename TElem, typename TDevice>
struct _PrincipalDataType<CategoryTags::Matrix, TElem, TDevice> {
    using type = Matrix<TElem, TDevice>;
};
template <typename TElem, typename TDevice>
struct _PrincipalDataType<CategoryTags::BatchScalar, TElem, TDevice> {
    using type = Batch<TElem, TDevice, CategoryTags::Scalar>;
};
template <typename TElem, typename TDevice>
struct _PrincipalDataType<CategoryTags::BatchMatrix, TElem, TDevice> {
    using type = Batch<TElem, TDevice, CategoryTags::Matrix>;
};

template <typename TCategory, typename TElem, typename TDevice>
using PrincipalDataType = typename _PrincipalDataType<TCategory, TElem, TDevice>::type;


}
