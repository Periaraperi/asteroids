@echo off

if exist build (
    echo Clearing build directory...
    rmdir /s /q build
)
