#pragma once

#include <cstdint>
#include <vector>

#include <glad/glad.h>

#include "opengl_errors.hpp"
#include "peria_logger.hpp"

enum class Buffer_Type {
    STATIC = 0,
    DYNAMIC
};

template<typename T>
class Vertex_Buffer {
public:
    // create static vbo and copy vertex_data into vbo _data
    Vertex_Buffer(const std::vector<T>& vertex_data)
        :_type{Buffer_Type::STATIC},
        _data{vertex_data}
    {
        PERIA_LOG("Static Vertex Buffer ctor()");
        GL_CALL(glGenBuffers(1, &_vbo));
        bind();
        GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(T)*_data.size(), _data.data(), GL_STATIC_DRAW));
    }

    // create static vbo and move vertex_data into vbo _data
    Vertex_Buffer(std::vector<T>&& vertex_data)
        :_type{Buffer_Type::STATIC},
        _data{std::move(vertex_data)}
    {
        PERIA_LOG("Static Vertex Buffer move ctor()");
        GL_CALL(glGenBuffers(1, &_vbo));
        bind();
        GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(T)*_data.size(), _data.data(), GL_STATIC_DRAW));
    }

    // create dynamic VBO of bytes
    Vertex_Buffer(std::size_t bytes)
        :_type{Buffer_Type::DYNAMIC}
    {
        PERIA_LOG("Dynamic Vertex Buffer ctor()");
        GL_CALL(glGenBuffers(1, &_vbo));
        bind();
        GL_CALL(glBufferData(GL_ARRAY_BUFFER, bytes, nullptr, GL_DYNAMIC_DRAW));
    }

    ~Vertex_Buffer()
    {
        PERIA_LOG("Vertex Buffer dtor()");
        GL_CALL(glDeleteBuffers(1, &_vbo));
    }

    void bind() const
    { GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, _vbo)); }

    void unbind() const
    { GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0)); }

    // populate sub region of dynamic vbo buffer.
    // start: offset into vbo buffer from where we start to copy
    // offset: offset into _data from where we copy
    // bytes: copy this many bytes
    void set_subdata(std::size_t start, std::size_t offset, std::size_t bytes)
    {
        PERIA_ASSERT(_type==Buffer_Type::DYNAMIC, "set_subdata() works only on dynamic buffer");
        GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, start, bytes, _data.data()+offset));
    }

    // dynamically add vertices to _data.
    // Use only on dynamic buffer
    void add_data(T&& vertex)
    {
        PERIA_ASSERT(_type==Buffer_Type::DYNAMIC, "add_data() works only on dynamic buffer");
        _data.push_back(std::forward<T>(vertex));
    }

    void clear_data()
    { _data.clear(); }

    bool data_empty() const
    { return _data.empty(); }

    std::size_t data_size() const
    { return _data.size(); }

    Vertex_Buffer(const Vertex_Buffer&) = delete;
    Vertex_Buffer& operator=(const Vertex_Buffer&) = delete;
    Vertex_Buffer(Vertex_Buffer&&) = delete;
    Vertex_Buffer& operator=(Vertex_Buffer&&) = delete;

private:
    uint32_t _vbo;
    Buffer_Type _type;
    std::vector<T> _data;
};
