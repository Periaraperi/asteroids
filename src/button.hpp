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
    void set_text(const std::string& str, const glm::vec2& pos, const glm::vec4 text_color, float scale=1.0f);

    [[nodiscard]]
    glm::vec2 button_pos() const;

    void draw(Graphics& g) const;

private:
    glm::vec2 _pos;
    glm::vec2 _size;
    float _border_thickness;
    glm::vec4 _fg_color;
    glm::vec4 _border_color;

    struct Text_Info {
        std::string text{};
        glm::vec2 pos;
        glm::vec4 text_color{0.0f, 0.0f, 0.0f, 1.0f};
        float text_scale{1.0f};
    };
    Text_Info _text;

    bool _is_hovered{false};
};
