#pragma once

#include <vector>
#include "transform.hpp"

class Graphics;

class Asteroid {
public:
    enum class Asteroid_Type {
        SMALL = 0,
        MEDIUM,
        LARGE,
    };

    Asteroid() = default;

    // pos - initial world pos.
    // dir_vector - normalized direction vector.
    Asteroid(Asteroid_Type asteroid_type, glm::vec2 pos, glm::vec2 dir_vector, uint8_t level_id);

    void update(float dt);
    void draw(Graphics& g, float alpha) const;

    void set_color(glm::vec4 color) 
    { _color = color; }

    void reset_color()
    { _color = {0.8f, 0.8f, 0.8f, 1.0f}; }

    [[nodiscard]]
    glm::vec2 get_world_pos() const;

    [[nodiscard]]
    bool dead() const;

    [[nodiscard]]
    uint8_t hp() const;

    void explode();

    void hit();

    [[nodiscard]]
    std::vector<glm::vec2> get_points_in_world() const;

    [[nodiscard]]
    std::vector<glm::vec2> get_points_in_world_interpolated(const Transform& interpolated_transform) const;

    [[nodiscard]]
    std::vector<Asteroid> split();

private:

    [[nodiscard]]
    std::vector<glm::vec2> init_asteroid_model(Asteroid_Type type);

private:
    Asteroid_Type _type;
    Transform _transform{};
    Transform _prev_transform{};

    glm::vec2 _velocity{};
    float _angle_rotation_speed{};

    uint8_t _hp; // this many hits to destroy
    uint8_t _level_id;
    bool _dead{};

    glm::vec4 _color = glm::vec4{0.8f, 0.8f, 0.8f, 1.0f};

    std::vector<glm::vec2> _asteroid_model;
};
