#pragma once

#include <glm/vec4.hpp>
#include <glm/vec2.hpp>
#include <string>

class Graphics;

class Button {
public:
    Button() = default;
    Button(const glm::vec4& pos_size, const glm::vec4& fg_color, const glm::vec4& border_color, float border_thickness);
    
    [[nodiscard]]
    bool is_hovered(float mx, float my);

    void set_colors(const glm::vec4& fg, const glm::vec4& border);
    void draw(Graphics& g) const;

private:
    glm::vec2 _pos;
    glm::vec2 _size;
    float _border_thickness;
    glm::vec4 _fg_color;
    glm::vec4 _border_color;

    bool _is_hovered{false};
};
