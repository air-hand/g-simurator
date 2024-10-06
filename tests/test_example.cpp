// https://qiita.com/utkamioka/items/cacb1001bd2abf605b15#%E6%89%8B%E9%A0%862-cmakeliststxt%E3%82%92%E3%83%97%E3%83%AD%E3%82%B8%E3%82%A7%E3%82%AF%E3%83%88%E3%81%AB%E5%90%88%E3%82%8F%E3%81%9B%E3%81%A6%E6%9B%B8%E3%81%8D%E6%8F%9B%E3%81%88

#include <stdexcept>
#include <gtest/gtest.h>

namespace
{

TEST(test_example, foobar)
{
    EXPECT_NE(std::string("foo"), std::string("bar"));
}

TEST(test_example, integer)
{
    EXPECT_EQ(1, 1);
}

}
