#!/bin/bash

# Ensure that you have the Conan and CMake commands in your PATH
# Install Conan: https://docs.conan.io/en/latest/installation.html
# Install CMake: https://cmake.org/download/

BUILD_TYPE="Debug"
BUILD_FOLDER="./build"
CONAN_PROFILE="conanprofile.toml"
CMAKE_TOOLCHAIN_FILE="conan_toolchain.cmake"
CMAKE_BUILD_GENERATOR="Unix Makefiles"

mkdir -p $BUILD_FOLDER

conan install . --output-folder=$BUILD_FOLDER --profile $CONAN_PROFILE --build=missing

cd $BUILD_FOLDER
cmake .. -G "$CMAKE_BUILD_GENERATOR" -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCMAKE_TOOLCHAIN_FILE=$CMAKE_TOOLCHAIN_FILE
cmake --build .

cd ..