#include "vertex_array.hpp"

#include "peria_logger.hpp"
#include "opengl_errors.hpp"

#include <glad/glad.h>

// get size in bytes from GL enum type names
uint8_t gl_type_bytes(GLenum type)
{
    switch (type) {
        case GL_BYTE: case GL_UNSIGNED_BYTE:
            return 1;
        case GL_SHORT: case GL_UNSIGNED_SHORT:
            return 2;
        case GL_INT: case GL_UNSIGNED_INT: case GL_FLOAT:
            return 4;
        case GL_DOUBLE:
            return 8;
    }

    return 0;
}

// creates new vertex array object
// and binds it to currently active one
Vertex_Array::Vertex_Array()
{
    PERIA_LOG("Created vao");
    GL_CALL(glGenVertexArrays(1, &_vao));
    bind();
}

// TODO: this class does not work unless I instantiate through
// smart or raw pointer. Think about this later.
// If can do on stack make move only type
//
//Vertex_Array::Vertex_Array(Vertex_Array&& rhs)
//    :_vao{rhs._vao}, _attributes{std::move(rhs._attributes)}
//{
//    rhs._vao = 0; // object moved so make its id 0
//}

Vertex_Array::~Vertex_Array()
{
    GL_CALL(glDeleteVertexArrays(1, &_vao));
    PERIA_LOG("Destroyed vao");
}

// stores vertex attributes
// vao must be bound before this call
// location index starts at 0 up to attrbute_count - 1
void Vertex_Array::add_attribute(int32_t count, uint32_t type, bool normalized, std::size_t stride)
{ _attributes.push_back({count, type, normalized, stride}); }

// sets the layout based on added attributes
// vao must be bound before this call
void Vertex_Array::set_layout()
{
    std::size_t offset = 0;
    for (std::size_t i{}; i<_attributes.size(); ++i) {
        const auto& a = _attributes[i];
        GL_CALL(glEnableVertexAttribArray(i));
        GL_CALL(glVertexAttribPointer(i, a.count, a.type, 
                    a.normalized ? GL_TRUE : GL_FALSE, 
                    a.stride, (const void*)offset));
        offset += (a.count * gl_type_bytes(a.type));
    }
}

void Vertex_Array::bind() const
{ GL_CALL(glBindVertexArray(_vao)); }

void Vertex_Array::unbind() const
{ GL_CALL(glBindVertexArray(0)); }
