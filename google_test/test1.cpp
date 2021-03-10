#include "gtest/gtest.h"

TEST(TestTheTest, Correct) { ASSERT_EQ(4, 4); }

TEST(TestTheTest, InCorrect) { ASSERT_EQ(1, 2); }