#pragma once

#include <vector>
#include "transform.hpp"

class Input_Manager;
class Graphics;

class Ship {
public:
    Ship(glm::vec2 world_pos);
    void update(Input_Manager& im, float dt);
    void draw(Graphics& g, float alpha) const;

    [[nodiscard]]
    std::vector<glm::vec2> get_points_in_world() const;

    [[nodiscard]]
    std::vector<glm::vec2> get_points_in_world_interpolated(const Transform& interpolated_transform) const;

    [[nodiscard]]
    glm::vec2 get_direction_vector() const;

    [[nodiscard]]
    float get_angle() const;

    void iframes(float step);

    void hit();
    
    [[nodiscard]]
    uint8_t hp() const;

    [[nodiscard]]
    bool is_invincible() const;

    void restart();

    void upgrade_max_health();
    void upgrade_speed();
    void upgrade_rotation_speed();
private:
    std::vector<glm::vec2> _ship_model;

    glm::vec2 _initial_pos;
    Transform _transform{};
    Transform _prev_transform{};

    glm::vec2 _velocity;
    uint8_t _max_hp{3};
    uint8_t _hp{3};

    float _iframe_duration{2.0f};
    float _speed{350.0f};
    float _rot_speed{150.0f};
    float _decceleration_speed;
    bool _invincible;
};
