#pragma once

#include <cstdint>

class Texture {
public:
    // Creates GLTexture from ttf bitmap buffer
    Texture(uint32_t width, uint32_t height, const void* bitmap_buffer_data);
    ~Texture();

    void bind(uint8_t tex_slot=0);

private:
    uint32_t _tex;

public:
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    Texture(Texture&&) = delete;
    Texture& operator=(Texture&&) = delete;
};
