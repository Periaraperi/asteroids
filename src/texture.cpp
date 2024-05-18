#include "texture.hpp"

#include <glad/glad.h>

#include "opengl_errors.hpp"
#include "peria_logger.hpp"

Texture::Texture(uint32_t width, uint32_t height, const void* bitmap_buffer_data)
{
    PERIA_LOG("Glyph Texture Ctor()");
    GL_CALL(glGenTextures(1, &_tex));
    bind();
    
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height,
                        0, GL_RED, GL_UNSIGNED_BYTE, 
                        bitmap_buffer_data));

    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
}

Texture::~Texture()
{
    PERIA_LOG("Texture Cleanup");
    GL_CALL(glDeleteTextures(1, &_tex));
}

void Texture::bind(uint8_t tex_slot)
{
    GL_CALL(glActiveTexture(GL_TEXTURE0+tex_slot));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, _tex));
}
