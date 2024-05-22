#pragma once

#include <cstdint>
#include <vector>

class Vertex_Array {
public:
    Vertex_Array();
    ~Vertex_Array();

    Vertex_Array(const Vertex_Array&) = delete;
    Vertex_Array& operator=(const Vertex_Array&) = delete;

    Vertex_Array(Vertex_Array&&) = delete;
    Vertex_Array& operator=(Vertex_Array&&) = delete;

    void add_attribute(int32_t count, uint32_t type, bool normalized, std::size_t stride);
    void set_layout();

    void bind() const;
    void unbind() const;
    
private:
    uint32_t _vao{};

    struct Vertex_Attribute {
        int32_t count;
        uint32_t type;
        bool normalized; 
        std::size_t stride;
    };
    std::vector<Vertex_Attribute> _attributes;
};

