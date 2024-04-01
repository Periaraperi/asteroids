cmake_minimum_required(VERSION 3.20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(asteroids)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(EXTERNAL_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external)

set(SRCS 
    main.cpp

    ${SRC_DIR}/graphics.cpp
    ${SRC_DIR}/opengl_errors.cpp
    ${SRC_DIR}/input_manager.cpp
    ${SRC_DIR}/shader.cpp
    ${SRC_DIR}/physics.cpp
    ${SRC_DIR}/game.cpp
    ${SRC_DIR}/ship.cpp

    ${EXTERNAL_INCLUDE_DIR}/glad/src/glad.c
)

add_executable(asteroids ${SRCS})
target_compile_features(asteroids PRIVATE cxx_std_17)
target_compile_options(asteroids PRIVATE -Wall -Wextra -Wpedantic -Wshadow)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(asteroids PRIVATE PERIA_DEBUG)
endif()

find_package(SDL2 REQUIRED)
if (${SDL2_FOUND})
    message(STATUS "SDL2 Package Found")
endif()

target_include_directories(asteroids 
    PRIVATE 
    ${SDL2_INCLUDE_DIRS} 
    ${SRC_DIR}

    ${EXTERNAL_INCLUDE_DIR}/glad/include
    ${EXTERNAL_INCLUDE_DIR}/glm/
)
target_link_libraries(asteroids PRIVATE ${SDL2_LIBRARIES})
