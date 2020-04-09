#include <gtest/gtest.h>
#include <vector>
#include <set>
#include <iostream>
#include <string>
#include <type_traits>
#include <memory>
#include <functional>
#include <utility/tinystl/variant.h>
#include <utility/tinystl/type_traits.h>

namespace unittests {
namespace test {
struct Variant : public ::testing::Test {};

struct Data {
    Data(const size_t data = 0) : data_(data) {
    }
    Data(const Data& data) {
        data_ = data.data_;
    }
    Data(Data&& data) {
        data_ = data.data_;
        data.data_ = -1;
    }
    ~Data() {
    }
    bool operator==(Data data) {
        return data.data_ == data_;
    }
    size_t data_ = 0;
};

TEST_F(Variant, contains) {
    bool ret = ::tinystl::Contains<int, int, double>::value;
    EXPECT_TRUE(ret);
    ret = ::tinystl::Contains<char, int, double>::value;
    EXPECT_TRUE(!ret);
}

TEST_F(Variant, constructor) {
    using V1 = ::tinystl::variant<int, double, ::std::string, Data>;
    V1 var1(12);
    EXPECT_TRUE(var1.is<int>() and var1 == 12);

    V1 var2(Data(12));
    EXPECT_TRUE(var2.is<Data>() and var2 == Data(12));
    Data data(12);
    V1 var22(data);
    EXPECT_TRUE(var22.is<Data>() and var22 == Data(12));

    V1 var3(::std::string("1234"));
    EXPECT_TRUE(var3.is<::std::string>() and var3 == ::std::string("1234"));

    V1 var4(var1);
    EXPECT_TRUE(var4.is<int>() and var4 == 12);

    V1 var5(::std::move(var2));
    EXPECT_TRUE(var5.is<Data>() and var5 == Data(12));
    EXPECT_TRUE(var2.is<void>());

    V1 var6(var5);
    EXPECT_TRUE(var6.is<Data>() and var6 == Data(12));
    EXPECT_TRUE(var5.is<Data>() and var5 == Data(12));
}

TEST_F(Variant, assign_op) {
    using V1 = ::tinystl::variant<int, double, ::std::string, Data>;
    V1 var1;
    Data data(11);
    var1 = data; 
    EXPECT_TRUE(var1.is<Data>() and var1 == Data(11));
    EXPECT_TRUE(data.data_ == 11);

    var1 = ::std::move(data);
    EXPECT_TRUE(var1.is<Data>() and var1 == Data(11));
    EXPECT_TRUE(data.data_ == -1);

    V1 var2;
    var2 = var1;
    EXPECT_TRUE(var1.is<Data>() and var1 == Data(11));
    EXPECT_TRUE(var2.is<Data>() and var2 == Data(11));

    var2 = ::std::move(var1);
    EXPECT_TRUE(var1.is<void>());
    EXPECT_TRUE(var2.is<Data>() and var2 == Data(11));
}

TEST_F(Variant, get) {
    using V1 = ::tinystl::variant<int, double, ::std::string, Data>;
    V1 var1;
    const Data data(11);
    var1 = data; 
    EXPECT_TRUE(var1.is<Data>() and var1 == Data(11));

    EXPECT_TRUE(var1.get<double>() == nullptr);
    EXPECT_TRUE(var1.get<Data>() != nullptr and var1.get<Data>()->data_ == 11);

}

TEST_F(Variant, type_get) {
    using V1 = ::tinystl::variant<int, double, ::std::string, Data>;
    EXPECT_TRUE(::std::type_index(typeid(typename V1::ValueType<0>)) == ::std::type_index(typeid(int)));
    EXPECT_TRUE(::std::type_index(typeid(typename V1::ValueType<3>)) == ::std::type_index(typeid(Data)));
}

TEST_F(Variant, function_trait) {
    using FunctionT = ::std::function<int(char)>;
    using GeneralFunctionT = int(char);
    typedef int(*Pointer)(char);

    EXPECT_TRUE(::tinystl::Function_Traits<::std::function<int(void)>>::ArgNum == 0);
    EXPECT_TRUE(::tinystl::Function_Traits<::std::function<int(int)>>::ArgNum == 1);
    
    using ExpectedRet = typename ::tinystl::Function_Traits<FunctionT>::Ret;
    EXPECT_TRUE(::std::type_index(typeid(ExpectedRet)) == ::std::type_index(typeid(int)));

    using ExpectedPointer = typename ::tinystl::Function_Traits<FunctionT>::Pointer;
    EXPECT_TRUE(::std::type_index(typeid(ExpectedPointer)) == ::std::type_index(typeid(Pointer)));

    using ExpectedGeneralFunctionT = typename ::tinystl::Function_Traits<FunctionT>::FunctionType;
    EXPECT_TRUE(::std::type_index(typeid(ExpectedGeneralFunctionT)) == ::std::type_index(typeid(GeneralFunctionT)));
}
}
}
