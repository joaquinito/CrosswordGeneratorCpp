# If non-existant, create folder 'bin'
if [ ! -d "bin" ]; then
    mkdir bin;
fi

# If non-existant, create folder 'build'
if [ ! -d "build" ]; then
    mkdir build;
fi

cd build

# If Google Test has not been cloned yet, clone from Github
if [ ! -d "googletest" ]; then
   git clone https://github.com/google/googletest.git;
fi

# Run CMake
cmake ..
# Run makefiles
make