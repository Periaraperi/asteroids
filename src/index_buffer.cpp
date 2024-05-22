#include "index_buffer.hpp"

#include <glad/glad.h>

#include "opengl_errors.hpp"
#include "peria_logger.hpp"

Index_Buffer::Index_Buffer(uint32_t index_count)
{
    PERIA_LOG("Index Buffer ctor()");
    GL_CALL(glGenBuffers(1, &_ibo));
    bind();

    _data.reserve(index_count);
    for (std::size_t i{}; i<index_count; i += 4) {
        _data.push_back(i);
        _data.push_back(i+1);
        _data.push_back(i+2);

        _data.push_back(i);
        _data.push_back(i+2);
        _data.push_back(i+3);
    }
    GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t)*_data.size(), _data.data(), GL_STATIC_DRAW));
}

Index_Buffer::~Index_Buffer()
{
    PERIA_LOG("Index Buffer dtor()");
    GL_CALL(glDeleteBuffers(1, &_ibo));
}

void Index_Buffer::bind() const
{ GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo)); }

void Index_Buffer::unbind() const
{ GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0)); }

std::size_t Index_Buffer::data_size() const
{ return _data.size(); }
