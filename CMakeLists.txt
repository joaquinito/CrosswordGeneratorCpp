# This CMakeLists file needs fixing - it will not work on first try
# if all Unit Test related commands are commented out, because the 
# GoogleTest project must be built.

cmake_minimum_required(VERSION 3.10)

# Project name
project(CrosswordsGenerator)

# Define macro "Headers" to reference all .hpp files 
set(Headers
    ../src/InputCheck.hpp
)

# Define macro "Sources" to reference all .cpp files 
set(Sources
    ../src/InputCheck.cpp
)

# Import GoogleTest from its Github repo
# Taken from: https://crascit.com/2015/07/25/cmake-gtest/
include(ExternalProject)
ExternalProject_Add(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/googletest
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    TEST_COMMAND ""
)

# Include the CMakeLists from GoogleTest
add_subdirectory(build/googletest/src/googletest)

# Create static library CrosswordsGenLib to be used by the unit tests
add_library(CrosswordsGenLib STATIC ${Sources} ${Headers})

# Define executable name and what sources/headers are linked - Unit Tests
add_executable(UnitTestsCrosswordsGen ../test/Test_InputCheck.cpp)

# Link GoogleTest and the CrosswordsGenLib to our unit tests executable
target_link_libraries(UnitTestsCrosswordsGen PUBLIC
    gtest_main
    CrosswordsGenLib
)    

# Define executable name and what sources/headers are linked - Main 
add_executable(CrosswordsGen src/main.cpp ${Sources} ${Headers})

# Set the directory in which the executable will be placed
set_target_properties(UnitTestsCrosswordsGen PROPERTIES RUNTIME_OUTPUT_DIRECTORY ../bin/ )
set_target_properties(CrosswordsGen PROPERTIES RUNTIME_OUTPUT_DIRECTORY ../bin/ )
