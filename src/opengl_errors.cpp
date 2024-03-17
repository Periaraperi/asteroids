#include "opengl_errors.hpp"

#include <glad/glad.h>
#include <string>
#include <cstring>
#include <iostream>

void log_error(const char* expr, const char* file, int line, const char* msg)
{
    std::cerr << expr << " failed\n";
    std::cerr << "FILE: " << file << '\n';
    std::cerr << "LINE: " << line << '\n';
    if (std::strcmp(msg, "") != 0) {
        std::cerr << "Msg: " << msg << '\n';
    }
}


// ============================= OPENGL ====================================
std::string gl_description(int err)
{
    switch (err) {
        case 1280: return "GL_INVALID_ENUM";
        case 1281: return "GL_INVALID_VALUE";
        case 1282: return "GL_INVALID_OPERATION";
        case 1283: return "GL_STACK_OVERFLOW";
        case 1284: return "GL_STACK_UNDERFLOW";
        case 1285: return "GL_OUT_OF_MEMORY";
        case 1286: return "GL_INVALID_FRAMEBUFFER_OPERATION";
    }
    return "";
}

void gl_clear_errors()
{
    while (glGetError() != GL_NO_ERROR);
}

bool gl_check_errors()
{
    while (auto err = glGetError()) {
        std::cerr << "[OPENGL Error]: " << err << " --> " << gl_description(err) << '\n';
        return true;
    }
    return false;
}


