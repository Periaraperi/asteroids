#include "texture.hpp"

#include <glad/glad.h>

#include "opengl_errors.hpp"
#include "peria_logger.hpp"

Texture::Texture(uint32_t width, uint32_t height, const void* bitmap_buffer_data)
    :_width{width}, _height{height}
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

Texture::Texture(uint32_t width, uint32_t height, int32_t internal_format, uint32_t format)
    :_width{width}, _height{height}, _buffer{std::vector<uint8_t>(width*height)}
{
    PERIA_LOG("Arbitrary Texture Ctor()");
    GL_CALL(glGenTextures(1, &_tex));
    bind();
    
    GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height,
                        0, format, GL_UNSIGNED_BYTE, 
                        _buffer.data()));

    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 4));
}

Texture::~Texture()
{
    PERIA_LOG("Texture Cleanup");
    GL_CALL(glDeleteTextures(1, &_tex));
}

void Texture::bind(uint8_t tex_slot) const
{
    GL_CALL(glActiveTexture(GL_TEXTURE0+tex_slot));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, _tex));
}

// expects bottom left corner of sub-rectangle. y-axis points up
void Texture::write_sub_texture(uint32_t xoffset, uint32_t yoffset,
                                uint32_t width, uint32_t height,
                                const void* data)
{
    bind();
    GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
    GL_CALL(glTexSubImage2D(GL_TEXTURE_2D, 0,
                            xoffset, yoffset, 
                            width, height, GL_RED,
                            GL_UNSIGNED_BYTE, data));

    GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 4));
}
