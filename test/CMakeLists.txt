cmake_minimum_required(VERSION 3.10)

set(Sources
    Test_InputCheck.cpp
)

add_executable(UnitTestsCrosswordsGenerator ${Sources})

target_link_libraries(UnitTestsCrosswordsGenerator PUBLIC
    gtest_main
    CrosswordsGenerator
)