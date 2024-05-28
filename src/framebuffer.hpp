#pragma once

#include <cstdint>
#include <memory>

#include "texture.hpp"

class Frame_Buffer {
public:
    Frame_Buffer(uint32_t width, uint32_t height);
    ~Frame_Buffer();

    void bind() const;
    void unbind() const;

    void bind_color_texture();

private:
    uint32_t _fbo;
    std::unique_ptr<Texture> _frame_buffer_texture;

public:
    Frame_Buffer(const Frame_Buffer&) = delete;
    Frame_Buffer& operator=(const Frame_Buffer&) = delete;
    Frame_Buffer(Frame_Buffer&&) = delete;
    Frame_Buffer& operator=(Frame_Buffer&&) = delete;
};
