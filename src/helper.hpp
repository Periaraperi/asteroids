#pragma once

#include <glm/vec2.hpp>

class Graphics;
class Input_Manager;

namespace peria {
    void update(Graphics& g, Input_Manager& im);
    void draw(Graphics& g, Input_Manager& im);

    [[nodiscard]]
    glm::vec2 get_mapped_mouse(Graphics& g, Input_Manager& im);
}
