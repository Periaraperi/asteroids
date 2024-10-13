#pragma once

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <vector>

#include "transform.hpp"

class Graphics;
class Asteroid;

class Homing_Bullet {
public:
    Homing_Bullet() = default;

    // world_pos is center of square, 2*radius is side_length
    Homing_Bullet(glm::vec2 world_pos, float radius, int target_index, glm::vec2 initial_direction, float initial_angle, glm::vec4 color);

    void update(float dt);

    void set_target_index(int target_index);
    void set_target_pos(glm::vec2 target_pos);

    static void set_damage(uint8_t dmg);
    static uint8_t get_damage();

    [[nodiscard]]
    int get_target_index() const;

    [[nodiscard]]
    std::vector<glm::vec2> get_world_points() const;

    [[nodiscard]]
    glm::vec2 get_world_pos() const;

    [[nodiscard]]
    bool dead() const;

    void explode();

    void draw(Graphics& g, float alpha) const;
private:
    Transform _transform;
    Transform _prev_transform;

    glm::vec2 _dir_vector;
    glm::vec4 _color;

    int _target_index;
    glm::vec2 _target_pos;

    static uint8_t _damage;

    bool _dead;
    float _timer{0.0f};
};
