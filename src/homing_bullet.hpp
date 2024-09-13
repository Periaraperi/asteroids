#pragma once

#include "asteroid.hpp"

class Graphics;

class Homing_Bullet {
public:
    Homing_Bullet() = default;

    // world_pos is center of square, 2*radius is side_length
    Homing_Bullet(glm::vec2 world_pos, float radius, int target_index, glm::vec2 initial_direction, glm::vec4 color);

    void update(float dt, glm::vec2 target);

    void set_target(int target_index);

    [[nodiscard]]
    int get_target() const;

    [[nodiscard]]
    std::vector<glm::vec2> get_world_points() const;

    [[nodiscard]]
    glm::vec2 get_world_pos() const;

    [[nodiscard]]
    bool dead() const;

    void explode();

    void draw(Graphics& g, float alpha) const;
private:
    glm::vec2 _pos;
    glm::vec2 _prev_pos;

    glm::vec2 _dir_vector;
    glm::vec4 _color;

    float _radius;
    int _target_index;

    bool _dead;
    float _timer{0.0f};
};
