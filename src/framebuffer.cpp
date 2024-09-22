#include "framebuffer.hpp"

#include <glad/glad.h>
#include "opengl_errors.hpp"
#include "peria_logger.hpp"

Frame_Buffer::Frame_Buffer(uint32_t width, uint32_t height, Frame_Buffer_Type type)
    :_type{type}
{
    PERIA_LOG("framebuffer ctor()");
    GL_CALL(glGenFramebuffers(1, &_fbo));
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, _fbo));
    
    if (_type == Frame_Buffer_Type::REGULAR) {
        _frame_buffer_texture = std::make_unique<Texture>(width, height, Texture::Texture_Type::REGULAR);
        _frame_buffer_texture->bind();
        GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, 
                GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 
                _frame_buffer_texture->get_id(), 0));
        unbind();
    }
    else if (_type == Frame_Buffer_Type::MULTI_SAMPLE) {
        _frame_buffer_texture = std::make_unique<Texture>(width, height, Texture::Texture_Type::MULTISAMPLE);
        _frame_buffer_texture->bind();
        GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, 
                GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, 
                _frame_buffer_texture->get_id(), 0));
        unbind();
    }
}

Frame_Buffer::~Frame_Buffer()
{
    PERIA_LOG("Deleting framebuffer");
    GL_CALL(glDeleteFramebuffers(1, &_fbo));
}

void Frame_Buffer::bind() const
{
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, _fbo));
    GL_CALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    GL_CALL(glClear(GL_COLOR_BUFFER_BIT));
    _frame_buffer_texture->bind();
    auto [w,h] = _frame_buffer_texture->get_dimensions();
    GL_CALL(glViewport(0, 0, w, h));
}

void Frame_Buffer::unbind() const
{
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void Frame_Buffer::bind_color_texture()
{
    _frame_buffer_texture->bind();
}

void Frame_Buffer::copy_to(Frame_Buffer* src, Frame_Buffer* dest)
{
    auto [w, h] = src->_frame_buffer_texture->get_dimensions();
    GL_CALL(glBindFramebuffer(GL_READ_FRAMEBUFFER, src->_fbo));
    GL_CALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dest->_fbo));
    GL_CALL(glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST));
    GL_CALL(glBindFramebuffer(GL_READ_FRAMEBUFFER, 0));
    GL_CALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
}
