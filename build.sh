#!/bin/sh -x

COMPILER_GCC="gcc"
COMPILER_CLANG="clang"

COMPILER_DEFAULT="$COMPILER_GCC"

if [ "$1" == "$COMPILER_GCC" ]; then
    COMPILER="$COMPILER_GCC"
elif [ "$1" == "$COMPILER_CLANG" ]; then
    COMPILER="$COMPILER_CLANG"
else
    COMPILER="$COMPILER_DEFAULT"
fi


BUILD_SYSTEM='Ninja'

if [ "$BUILD_SYSTEM" == 'make' ]; then
    GENERATOR=-G"Unix Makefiles"
    MAKE_PROGRAM='-DCMAKE_MAKE_PROGRAM=mingw32-make.exe'
else
    GENERATOR='-GNinja'
    MAKE_PROGRAM=''
fi

CMAKE_CXX_FLAGS_COMMON='-DCMAKE_CXX_FLAGS=" -static -static-libstdc++ -static-libgcc -std=c++17 -fconcepts -O3 -pedantic -pedantic-errors -Wall -Wextra -Werror -Waggregate-return -Wshadow -Weffc++ Wcast-align -Wcast-qual -Wchar-subscripts  -Wcomment -Wconversion -Wfloat-equal  -Wformat  -Wformat=2 -Wformat-nonliteral -Wformat-security -Wformat-y2k -Wimplicit  -Wimport  -Winit-self  -Winline -Winvalid-pch -Wunsafe-loop-optimizations  -Wlong-long -Wmissing-braces -Wmissing-field-initializers -Wmissing-format-attribute"'
if [ "$COMPILER" == "$COMPILER_GCC" ]; then
    C_COMPILER='gcc.exe'
    CXX_COMPILER='g++.exe'
    LINKER='ld.exe'
    CMAKE_CXX_FLAGS="$CMAKE_CXX_FLAGS_COMMON"
    BUILD_DIR=../xll-gcc
elif [ "$COMPILER" == "$COMPILER_CLANG" ]; then
     C_COMPILER='clang.exe'
     CXX_COMPILER='clang++.exe'
     LINKER='lld.exe'
    CMAKE_CXX_FLAGS="-stdlib=libc++ $CMAKE_CXX_FLAGS_COMMON"
    BUILD_DIR=../xll-clang
fi

SOURCE_DIR=./
INSTALL_DIR=../xll-bin
XLW_DIR='C:/NotBackedup/Programs/xlw'
XLW_INCLUDE_DIR="$XLW_DIR/xlw/include"
XLW_LIB="xlw-gcc-s-5_0_2f0"
XLW_LIB_DIR="$XLW_DIR/xlw/lib"

BUILD_TYPE='Release'
INTERFACEGENERATOR_EXECUTABLE="$XLW_DIR/xlw/build/codeblocks-gcc/bin/$BUILD_TYPE/InterfaceGenerator.exe"
cmake -E make_directory $BUILD_DIR

cmake -E time cmake -H$SOURCE_DIR -B$BUILD_DIR "$GENERATOR" $MAKE_PROGRAM -DCMAKE_C_COMPILER=$C_COMPILER -DCMAKE_CXX_COMPILER=$CXX_COMPILER   -DCMAKE_LINKER:FILEPATH=$LINKER -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR" -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DXLW_INCLUDE_DIR="$XLW_INCLUDE_DIR" -DXLW_LIB_DIR=$XLW_LIB_DIR -DXLW_LIB=$XLW_LIB -DCMAKE_CXX_FLAGS="$CMAKE_CXX_FLAGS" -DINTERFACEGENERATOR_EXECUTABLE="$INTERFACEGENERATOR_EXECUTABLE"
cmake -E time cmake --build "$BUILD_DIR" --config Release --target install
