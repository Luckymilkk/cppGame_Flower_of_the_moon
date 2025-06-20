#include "gtest/gtest.h"
#include "../utils.h"

TEST(UtilsTest, SplitString) {
    std::string s = "a, b, c";
    auto tokens = splitString(s, ',');
    ASSERT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0], "a");
    EXPECT_EQ(tokens[1], "b");
    EXPECT_EQ(tokens[2], "c");
}

TEST(UtilsTest, TrimString) {
    EXPECT_EQ(trimString("  hello  "), "hello");
    EXPECT_EQ(trimString("\tfoo\n"), "foo");
    EXPECT_EQ(trimString("bar"), "bar");
    EXPECT_EQ(trimString("   "), "");
} 