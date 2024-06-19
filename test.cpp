#include <gtest/gtest.h>
#include "solution.h"


class SolutionTest : public ::testing::Test {
protected:
    Solution solution;
};


TEST_F(SolutionTest, Test1) {
    std::string input = "3[a]2[bc]";
    std::string expected = "aaabcbc";

    std::string result = solution.decodeString(input);

    EXPECT_EQ(result, expected);
}

TEST_F(SolutionTest, Test2) {
    std::string input = "3[a2[c]]";
    std::string expected = "accaccacc";

    std::string result = solution.decodeString(input);

    EXPECT_EQ(result, expected);
}


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
