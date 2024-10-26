#include "button.hpp"
#include "graphics.hpp"

Button::Button(const glm::vec4& pos_size, const glm::vec4& fg_color, const glm::vec4& border_color, float border_thickness)
    :_pos{glm::vec2{pos_size.x, pos_size.y}}, 
    _size{glm::vec2{pos_size.z, pos_size.w}},
    _border_thickness{border_thickness},
    _fg_color{fg_color}, _border_color{border_color}
{}

bool Button::is_hovered(float mx, float my)
{
    return (_is_hovered = mx >= _pos.x && mx <= _pos.x+_size.x &&
           my <= _pos.y && my >= _pos.y-_size.y); 
}

void Button::set_colors(const glm::vec4& fg, const glm::vec4& border)
{
    _fg_color = fg;
    _border_color = border;
}

void Button::set_text(const std::string& str, const glm::vec2& pos, const glm::vec4 text_color, float scale)
{
    _text.text = str;
    _text.pos = pos;
    _text.text_color = text_color;
    _text.text_scale = scale;
}

glm::vec2 Button::button_pos() const
{ return _pos; }

void Button::draw(Graphics& g) const
{
    g.draw_rect(_pos, _size, _fg_color);
    if (_border_thickness > 0.0f) {
        g.draw_rect({_pos.x, _pos.y+_border_thickness}, {_size.x, _border_thickness}, _border_color);
        g.draw_rect({_pos.x, _pos.y-_size.y}, {_size.x, _border_thickness}, _border_color);
        g.draw_rect({_pos.x-_border_thickness, _pos.y+_border_thickness}, {_border_thickness, 2*_border_thickness+_size.y}, _border_color);
        g.draw_rect({_pos.x+_size.x, _pos.y+_border_thickness}, {_border_thickness, 2*_border_thickness+_size.y}, _border_color);
    }

    if (!_text.text.empty()) {
        g.draw_text(_text.text, _text.pos, _text.text_color, 48, _text.text_scale);
    }
}
