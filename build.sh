#!/bin/bash

BUILD_DIR=build

if [ -d "$BUILD_DIR" ]; then
    rm -rf $BUILD_DIR
fi

mkdir $BUILD_DIR
cd $BUILD_DIR

cmake ..

make

make install

# 运行测试（如果需要）
# ./test/test_NMEAParser
# ctest --output-on-failure
