#pragma once

#include <cstdint>
#include <memory>

#include "texture.hpp"

class Frame_Buffer {
public:
    enum class Frame_Buffer_Type {
        REGULAR = 0,
        MULTI_SAMPLE,
    };
    Frame_Buffer(uint32_t width, uint32_t height, Frame_Buffer_Type type);
    ~Frame_Buffer();

    void bind() const;
    void unbind() const;

    void bind_color_texture();

    static void copy_to(Frame_Buffer* src, Frame_Buffer* dest);
private:
    uint32_t _fbo;
    Frame_Buffer_Type _type;
    std::unique_ptr<Texture> _frame_buffer_texture;

public:
    Frame_Buffer(const Frame_Buffer&) = delete;
    Frame_Buffer& operator=(const Frame_Buffer&) = delete;
    Frame_Buffer(Frame_Buffer&&) = delete;
    Frame_Buffer& operator=(Frame_Buffer&&) = delete;
};
