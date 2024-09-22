#pragma once

#include <cstdint>
#include <vector>

class Texture {
public:
    enum class Texture_Type {
        REGULAR = 0,
        MULTISAMPLE
    };

    // Creates GLTexture from ttf bitmap buffer
    Texture(uint32_t width, uint32_t height, const void* bitmap_buffer_data);
    
    // Create texture of arbitrary size
    Texture(uint32_t width, uint32_t height, int32_t internal_format, uint32_t format);

    // for framebuffer.
    Texture(uint32_t width, uint32_t height, Texture_Type type);

    ~Texture();

    void bind(uint8_t tex_slot=0) const;

    void write_sub_texture(uint32_t xoffset, uint32_t yoffset,
                           uint32_t width, uint32_t height,
                           const void* data);
    [[nodiscard]] uint32_t get_id() 
    { return _tex; }

    [[nodiscard]]
    std::pair<uint32_t, uint32_t> get_dimensions()
    { return {_width, _height}; }
private:
    uint32_t _tex; // gl texture id

    uint32_t _width;
    uint32_t _height;
    Texture_Type _type;

    // unsigned byte buffer for texture
    std::vector<uint8_t> _buffer;
public:
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    Texture(Texture&&) = delete;
    Texture& operator=(Texture&&) = delete;
};
