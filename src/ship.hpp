#pragma once

#include <vector>
#include "transform.hpp"

class Input_Manager;
class Graphics;

class Ship {
public:
    explicit Ship(glm::vec2 world_pos);
    void update(Graphics& g, Input_Manager& im, float dt);
    void draw(Graphics& g) const;

    [[nodiscard]]
    std::vector<glm::vec2> get_points_in_world() const;

    [[nodiscard]]
    glm::vec2 get_direction_vector() const;

private:
    std::vector<glm::vec2> _ship_model;

    Transform _transform;
    glm::vec2 _velocity;

};
