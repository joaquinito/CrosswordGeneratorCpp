if [ ! -d "bin" ]; then
    mkdir bin;
fi

cd src
g++ *.cpp *.hpp -o ../bin/CrosswordGen