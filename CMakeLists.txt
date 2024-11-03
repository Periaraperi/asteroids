cmake_minimum_required(VERSION 3.20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(asteroids)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(EXTERNAL_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external)

set(SRCS 
    main.cpp

    ${SRC_DIR}/graphics.cpp
    ${SRC_DIR}/vertex_array.cpp
    ${SRC_DIR}/index_buffer.cpp
    ${SRC_DIR}/input_manager.cpp
    ${SRC_DIR}/texture.cpp
    ${SRC_DIR}/shader.cpp
    ${SRC_DIR}/opengl_errors.cpp
    ${SRC_DIR}/physics.cpp
    ${SRC_DIR}/framebuffer.cpp
    ${SRC_DIR}/button.cpp

    # game specific
    ${SRC_DIR}/game.cpp
    ${SRC_DIR}/ship.cpp
    ${SRC_DIR}/asteroid.cpp
    ${SRC_DIR}/bullet.cpp
    ${SRC_DIR}/homing_bullet.cpp
    ${SRC_DIR}/weapons.cpp

    #helper for generating asteroids
    ${SRC_DIR}/helper.cpp

    #external
    ${EXTERNAL_INCLUDE_DIR}/glad/src/glad.c
)

if (UNIX) # we use GNU Compiler on linux
    add_executable(asteroids ${SRCS})
    target_compile_options(asteroids PRIVATE -Wall -Wextra -Wpedantic -Wshadow)

    # SDL2 and Freetype must be installed on your linux distro
    find_package(SDL2 REQUIRED)
    if (${SDL2_FOUND})
        message(STATUS "SDL2 Package Found")
    endif()

    find_package(Freetype REQUIRED)
    if (${FREETYPE_FOUND})
        message(STATUS "Freetype Package Found")
    endif()

    target_include_directories(asteroids 
        PRIVATE 
        ${SDL2_INCLUDE_DIRS} 
        ${SRC_DIR}

        ${EXTERNAL_INCLUDE_DIR}/glad/include
        ${EXTERNAL_INCLUDE_DIR}/glm/
        ${FREETYPE_INCLUDE_DIRS}
    )
    target_link_libraries(asteroids PRIVATE ${SDL2_LIBRARIES} ${FREETYPE_LIBRARIES})

elseif (WIN32) # currently setup for MSVC compiler
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_executable(asteroids ${SRCS})
    else ()
        add_executable(asteroids WIN32 ${SRCS})
    endif()
    target_compile_options(asteroids PRIVATE /W4) 

    # on windows link sdl2 and freetype from libs provided in external directory
    target_include_directories(asteroids 
        PRIVATE 
        ${SRC_DIR}

        ${EXTERNAL_INCLUDE_DIR}/glad/include
        ${EXTERNAL_INCLUDE_DIR}/glm/
        ${EXTERNAL_INCLUDE_DIR}/sdl2/include
        ${EXTERNAL_INCLUDE_DIR}/freetype/include
    )
    target_link_libraries(asteroids PRIVATE 
        ${EXTERNAL_INCLUDE_DIR}/sdl2/lib/x64/SDL2.lib
        ${EXTERNAL_INCLUDE_DIR}/sdl2/lib/x64/SDL2main.lib
        ${EXTERNAL_INCLUDE_DIR}/freetype/lib/freetype.lib)
endif()

target_compile_features(asteroids PRIVATE cxx_std_20)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(asteroids PRIVATE PERIA_DEBUG)
endif()

# during build copy res folder
if (UNIX)
    set(BUILD_OUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build/debug)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        set(BUILD_OUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build/release)
    endif()
elseif (WIN32) # when using MSVC with vs solutions output directory and structure is different. Just manually hack this.
    set(BUILD_OUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build/debug/Debug)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        set(BUILD_OUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build/release/Release)
    endif()
endif()


add_custom_command(
    TARGET asteroids POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/external/sdl2/lib/x64/SDL2.dll ${BUILD_OUT_DIR}
)
add_custom_command(
    TARGET asteroids POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/res ${BUILD_OUT_DIR}/res
)

#install stuff
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR})
install(TARGETS asteroids
        CONFIGURATIONS Release
        RUNTIME DESTINATION bin)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/res/ DESTINATION bin/res CONFIGURATIONS Release)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/external/sdl2/lib/x64/SDL2.dll DESTINATION bin CONFIGURATIONS Release)
