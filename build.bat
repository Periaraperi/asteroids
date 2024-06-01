@echo off

set build_type=%1

if "%build_type%"=="--debug" (
    echo Building debug build
    cmake --build .\build\debug\ --config Debug
) else if "%build_type%"=="--release" (
    echo Building release build
    cmake --build .\build\release\ --config Release
) else (
    echo Must specify build type
)
