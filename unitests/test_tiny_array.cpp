#include <gtest/gtest.h>
#include <utility/tinystl/array.hpp>
namespace tinystl {
namespace test {
struct array_test : public ::testing::Test {};
TEST_F(array_test, base) {
    tinystl::array<int, 10> arr; 
    EXPECT_TRUE(arr.data() == arr.begin());
    EXPECT_TRUE((arr.end() - arr.begin())== 10);
    arr[2] = 1;
    EXPECT_TRUE(arr[2] == 1); 
    arr.fill(2);
    EXPECT_TRUE(arr[2] == 2);

    tinystl::array<int, 20> arr1(5);
    for (auto ite = arr1.begin(); ite != arr1.end(); ++ite) { EXPECT_TRUE(*ite == 5); } 

    tinystl::array<int, 0> arr2;
    for (auto ite = arr2.begin(); ite != arr2.end(); ++ite) { EXPECT_TRUE(*ite == 5); }
}
}
}
