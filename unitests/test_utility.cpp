#include <gtest/gtest.h>
#include <utility/traits.h>
#include <utility/ostream_container.h>

namespace tinynn {
namespace unittests {
struct pretty_printer : public ::testing::Test {};
struct TnotWritable {};
struct TWritable {}; 
::std::ostream& operator<< (::std::ostream& os, const TWritable& tt) { 
    os << "writable"; 
    return os;
}

TEST_F( pretty_printer, vector) {
    {   //string or char
        ::std::stringstream ss;
        PrettyPrinter printer(ss);
        printer.print("1234");
        EXPECT_TRUE(ss.str() == "\"1234\"\n");
        ss.str(""); printer.print("1234", 12, '1');
        EXPECT_TRUE(ss.str() == "\"1234\" 12 \'1\'\n");
    }
    {   //interger
        ::std::stringstream ss;
        PrettyPrinter printer(ss);
        printer.print(1.12);
        EXPECT_TRUE(ss.str() == "1.12\n");
        ss.str(""); printer.print(122);
        EXPECT_TRUE(ss.str() == "122\n");
    }
    {   //pointer
        ::std::stringstream ss;
        PrettyPrinter printer(ss);
        printer.print(nullptr);
        EXPECT_TRUE(ss.str() == "nullptr\n");
        ss.str("");
        TnotWritable* tp = (TnotWritable*)(0x111111);
        printer.print(tp);
        EXPECT_TRUE(ss.str().find("TnotWritable* at 0x111111") != ::std::string::npos);
        tp = nullptr; ss.str(""); printer.print(tp);
        EXPECT_TRUE(ss.str() == "nullptr\n");
    }
    {   //class
        TnotWritable tt;
        ::std::stringstream ss;
        PrettyPrinter printer(ss);
        printer.print(tt);
        EXPECT_TRUE(ss.str().find("<Object") != ::std::string::npos);
        TWritable tt1; ss.str("");
        printer.print(tt1);
        EXPECT_TRUE(ss.str() == "writable\n");
    }
    {   //container
        ::std::vector<int> vec;
        EXPECT_TRUE(is_container_v<decltype(vec)> == true);
        ::std::stringstream ss;
        PrettyPrinter printer(ss);
        printer.print(vec);
        EXPECT_TRUE(ss.str() == "[]\n");
        vec.push_back(1); ss.str("");
        printer.print(vec);
        EXPECT_TRUE(ss.str() == "[1]\n");
        ::std::vector<::std::string> vec_str = {"1234"};
        ss.str("");
        printer.print(vec_str);
        EXPECT_TRUE(ss.str() == "[\"1234\"]\n");
        ::std::vector<::std::vector<int>> vec_vec = {{1234}};
        ss.str("");
        printer.print(vec_vec);
        EXPECT_TRUE(ss.str() == "[[1234]]\n");
        ::std::vector<int> vec1 = {1,2,3,4};
        ss.str("");
        printer.print(vec1);
        EXPECT_TRUE(ss.str() == "[1, 2, 3, 4]\n");
        ::std::vector<::std::string> vec_str1 = {"1","2","3"};
        ss.str("");
        printer.print(vec_str1);
        EXPECT_TRUE(ss.str() == "[\"1\", \"2\", \"3\"]\n");
        ::std::vector<::std::vector<int>> vec_vec1 = {{1,2}, {5,6,7}, {1}};
        ss.str("");
        printer.print(vec_vec1);
        ::std::cout << ss.str();
        EXPECT_TRUE(ss.str() == "[\n   [1, 2],\n   [5, 6, 7],\n   [1]\n]\n");
        ::std::vector<::std::vector<::std::vector<::std::string>>> vec_vec_str = {{{"123", "45"}, {"78"}},{{"123", "45"}, {"78"}}};
        ss.str("");
        printer.print(vec_vec_str);
        ::std::cout << ss.str();
        ::std::set<int> set; ss.str("");
        printer.print(set);
        EXPECT_TRUE(ss.str() == "{}\n");
        set.insert(1); ss.str("");
        printer.print(set);
        EXPECT_TRUE(ss.str() == "{1}\n");
        ::std::vector<::std::set<int>> vec_set = {{1,3,2}, {5,6}};
        ss.str("");
        printer.print(vec_set);
        EXPECT_TRUE(ss.str() == "[\n   {1, 2, 3},\n   {5, 6}\n]\n");
        ::std::cout << ss.str();
        ::std::pair<int, int> pair = {1,4}; ss.str("");
        printer.print(pair);
        EXPECT_TRUE(ss.str() == "1 : 4\n");
        ::std::map<::std::string, int> map = {{"1",1}, {"2",2}}; ss.str("");
        printer.print(map);
        EXPECT_TRUE(ss.str() == "{\"1\" : 1, \"2\" : 2}\n");
        ::std::map<::std::string, ::std::vector<int>> map_vec 
        = {{"1", {1 , 2, 3}}, {"2", {5, 6, 7}}}; ss.str("");
        printer.print(map_vec);
        EXPECT_TRUE(ss.str() == "{\"1\" : [1, 2, 3], \"2\" : [5, 6, 7]}\n");
        ::std::map<::std::string, ::std::map<int, ::std::vector<int>>> map_map = 
        {{"1", {{1, {1,2,3}}}}, {"2", {{2, {2,2,3}}}}, {"3", {{3, {3,2,3}}}}}; 
        ss.str("");
        printer.print(map_map);
        ::std::cout << ss.str();
        ::std::shared_ptr<int> ptr = ::std::make_shared<int>(); ss.str("");
        printer.print(ptr);
        ::std::cout << ss.str();
    }
}

struct traits : public ::testing::Test {};
TEST_F(traits, is_pair) {
    EXPECT_TRUE((is_pair_v<int> == false)); 
    EXPECT_TRUE((is_pair_v<::std::pair<int, int>> == true));   
    EXPECT_TRUE((is_pair_v<std::pair<std::pair<int, int>, char>> == true));   
}

TEST_F(traits, is_specialization) {
    EXPECT_TRUE((is_specialization_v<::std::vector<int>, ::std::vector> == true)); 
    EXPECT_TRUE((is_specialization_v<::std::vector<::std::vector<int>>, ::std::vector> == true)); 
    EXPECT_TRUE((is_specialization_v<::std::set<int>, ::std::vector> == false)); 
    EXPECT_TRUE((is_specialization_v<::std::string, ::std::vector> == false)); 
    EXPECT_TRUE((is_specialization_v<int, ::std::vector> == false)); 
    EXPECT_TRUE((is_specialization_v<::std::map<int, char>, ::std::map> == true));
}

TEST_F(traits, memberfunction) {
    float aa = 11;
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

    EXPECT_TRUE((is_to_stream_writable<int>::value == true));
    EXPECT_TRUE((is_to_stream_writable<TestClazzC>::value == false));
}

TEST_F(traits, gettypename) {
    int i = 1;
    const int ii = 1;
    int&& iii = 1;
    EXPECT_TRUE(get_type_name_by_val(i) == "int");
    EXPECT_TRUE(get_type_name_by_val(ii) == "int");
    EXPECT_TRUE(get_type_name_by_type<decltype(ii)>() == "const int");
    EXPECT_TRUE(get_type_name_by_type<const int&>() == "const int&");
    EXPECT_TRUE(get_type_name_by_type<decltype(iii)>() == "int&&");
}

}
}
