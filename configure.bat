@echo off

if not exist build (
    mkdir build
    mkdir build\debug
    mkdir build\release
)

echo Configuring Debug build...
cmake -DCMAKE_BUILD_TYPE=Debug -B build\debug\ -S .

echo Configuring Release build...
cmake -DCMAKE_BUILD_TYPE=Release -B build\release -S .
