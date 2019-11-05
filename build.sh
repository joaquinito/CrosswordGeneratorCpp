if [ ! -d "bin" ]; then
    mkdir bin;
fi

cd build
cmake ..
make