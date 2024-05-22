#pragma once

#include <cstdint>
#include <vector>

class Index_Buffer {
public:

    explicit Index_Buffer(uint32_t index_count);
    ~Index_Buffer();

    void bind() const;

    void unbind() const;

    std::size_t data_size() const;

    Index_Buffer(const Index_Buffer&) = delete;
    Index_Buffer& operator=(const Index_Buffer&) = delete;
    Index_Buffer(Index_Buffer&&) = delete;
    Index_Buffer& operator=(Index_Buffer&&) = delete;

private:
    uint32_t _ibo;
    std::vector<uint32_t> _data;
};
