#include <register.h>
#include <utility/traits.h>
#include <utility/ostream_contrainer.h>

using namespace test_dup;
namespace tinynn {
namespace unittests {

Register_Func(output_contrainer, vector) {
    {
        ::std::vector<int> vec;
        ::std::stringstream ss;
        ss << vec;
        EXPECT_TRUE(ss.str() == "{}");  
        vec = {1, 2, 3};  
        ::std::stringstream ss1;
        ss1 << vec;
        EXPECT_TRUE(ss1.str() == "{1, 2, 3}");  
        ::std::vector<::std::vector<int>> vecs = {{1, 2, 3}, {1, 4, 6}};
        ::std::stringstream ss2;
        ss2 << vecs;
        EXPECT_TRUE(ss2.str() == "{{1, 2, 3}, {1, 4, 6}}");  
    }
    {
        ::std::vector<char> vec = {'a', 'b'};
        ::std::stringstream ss;
        ss << vec;
        EXPECT_TRUE(ss.str() == "{\'a\', \'b\'}");  
        ::std::vector<::std::string> vec1 = {"ab", "mn"};
        ::std::stringstream ss1;
        ss1 << vec1;
        EXPECT_TRUE(ss1.str() == "{\"ab\", \"mn\"}");  
        ::std::vector<::std::pair<::std::pair<int, int>, char>> vec2 = {{{1,1}, '1'}, {{2,3}, '2'}};
        ::std::stringstream ss2;
        ss2 << vec2;
        EXPECT_TRUE(ss2.str() == "{((1 -> 1) -> \'1\'), ((2 -> 3) -> \'2\')}");  
    }
    {
        ::std::set<char> vec = {'a', 'b'};
        ::std::stringstream ss;
        ss << vec;
        EXPECT_TRUE(ss.str() == "{\'a\', \'b\'}");  
        ::std::set<char*> vec1 = {"123", "b"};
        ::std::stringstream ss1;
        ss1 << vec1;
        EXPECT_TRUE(ss1.str() == "{\"123\", \"b\"}");  
    }
}

Register_Func(traits, is_pair) {
    EXPECT_TRUE((is_pair_v<int> == false)); 
    EXPECT_TRUE((is_pair_v<::std::pair<int, int>> == true));   
    EXPECT_TRUE((is_pair_v<std::pair<std::pair<int, int>, char>> == true));   
}

Register_Func(traits, memberfunction) {
    struct TestClazzA {
        void initialize() {}
    };
    struct TestClazzB {
        void not_initialize() {}
    };
    struct TestClazzC {
        void initialize(int) {}
    };
    EXPECT_TRUE((has_member_initialize_v<TestClazzA> == true));
    EXPECT_TRUE((has_member_initialize_v<TestClazzB> == false));
    EXPECT_TRUE((has_member_initialize_v<TestClazzC> == true));

    //EXPECT_TRUE((has_output_stream_v<int> == true));
}

Register_Func(traits, gettypename) {
    int i = 1;
    const int ii = 1;
    int&& iii = 1;
    EXPECT_TRUE(get_type_name_by_val(i) == "int");
    EXPECT_TRUE(get_type_name_by_type<decltype(ii)>() == "const int");
    EXPECT_TRUE(get_type_name_by_type<const int&>() == "const int&");
    EXPECT_TRUE(get_type_name_by_type<decltype(iii)>() == "int&&");
}

}
}