#include <gtest/gtest.h>
#include <utility/fs_utils.h>
namespace tinynn {
namespace test {
struct fsutils_test : public ::testing::Test {};
TEST_F(fsutils_test, read) {
    ::std::vector<::std::string> converters;
    {
        auto filefd = FsUtils::open("./data/name.dat", "r", converters);
        EXPECT_TRUE(filefd != nullptr);
        int linenum = 0;
        auto line_processor = [&](char* buffer, const size_t size) {
            linenum++;
            if (linenum == 0) { 
                EXPECT_TRUE(size == 9); 
                EXPECT_TRUE(strncmp("rain chao", buffer, size) == 0); 
            }
        };
        FsUtils::read(filefd, line_processor);
        EXPECT_TRUE(linenum == 2); 
    }
    {
        auto filefd = FsUtils::open("./data/name.dat.tar.gz", "r", converters);
        EXPECT_TRUE(filefd != nullptr);
        int linenum = 0;
        auto line_processor = [&](char* buffer, const size_t size) {
            linenum++;
            if (linenum == 0) { 
                EXPECT_TRUE(size == 9); 
                EXPECT_TRUE(strncmp("rain chao", buffer, size) == 0); 
            }
        };
        FsUtils::read(filefd, line_processor);
        EXPECT_TRUE(linenum == 2); 
    }
    {
        converters.emplace_back("python ./script/sample_loss_weight.py");
        auto filefd = FsUtils::open("./data/name.dat", "r", converters);
        EXPECT_TRUE(filefd != nullptr);
        int linenum = 0;
        auto line_processor = [&](char* buffer, const size_t size) {
            linenum++;
            if (linenum == 0) { 
                EXPECT_TRUE(size == 4); 
                EXPECT_TRUE(strncmp("rain", buffer, size) == 0); 
            }
        };
        FsUtils::read(filefd, line_processor);
        EXPECT_TRUE(linenum == 2); 
    }


}
TEST_F(fsutils_test, write) {
    ::std::vector<::std::string> converters;
    {
        converters.emplace_back("python ./script/sample_loss_weight.py");
        auto filefd = FsUtils::open("./data/name_tmp.dat", "w", converters);
        EXPECT_TRUE(filefd != nullptr);
        EXPECT_TRUE(FsUtils::write(filefd, ::std::string("rain chao")) == 9);
        EXPECT_TRUE(FsUtils::write(filefd, ::std::string("chao rain")) == 9);
    }
    {
        converters.clear();  
        auto filefd = FsUtils::open("./data/name_tmp.dat", "r", converters);
        EXPECT_TRUE(filefd != nullptr);
        int linenum = 0;
        auto line_processor = [&](char* buffer, const size_t size) {
            linenum++;
            if (linenum == 0) { 
                EXPECT_TRUE(size == 4); 
                EXPECT_TRUE(strncmp("rain", buffer, size) == 0); 
            }
        };
        FsUtils::read(filefd, line_processor);
        EXPECT_TRUE(linenum == 2); 
    }
}
}
}
