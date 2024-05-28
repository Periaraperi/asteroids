#include "framebuffer.hpp"

#include <glad/glad.h>
#include "opengl_errors.hpp"
#include "peria_logger.hpp"

Frame_Buffer::Frame_Buffer(uint32_t width, uint32_t height)
{
    PERIA_LOG("framebuffer ctor()");
    GL_CALL(glGenFramebuffers(1, &_fbo));
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, _fbo));
    _frame_buffer_texture = std::make_unique<Texture>(width, height);
    _frame_buffer_texture->bind();
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, 
                GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 
                _frame_buffer_texture->get_id(), 0));
    unbind();
}

Frame_Buffer::~Frame_Buffer()
{
    PERIA_LOG("Deleting framebuffer");
    GL_CALL(glDeleteFramebuffers(1, &_fbo));
}

void Frame_Buffer::bind() const
{
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, _fbo));
    GL_CALL(glClearColor(0,0,0,1));
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
