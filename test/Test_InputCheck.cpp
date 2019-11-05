#include "gtest/gtest.h"
#include "../src/InputCheck.hpp"

TEST(Tests_CheckNumOfArgs, NegativeTest1)
{
    EXPECT_EQ(IsNumOfArgsValid(1), STATUS_NOK);
}

TEST(Tests_CheckNumOfArgs, NegativeTest2)
{
    EXPECT_EQ(IsNumOfArgsValid(2), STATUS_NOK);
}

TEST(Tests_CheckNumOfArgs, PositiveTest1)
{
    EXPECT_EQ(IsNumOfArgsValid(3), STATUS_OK);
}

TEST(Tests_CheckNumOfArgs, PositiveTest2)
{
    EXPECT_EQ(IsNumOfArgsValid(4), STATUS_OK);
}