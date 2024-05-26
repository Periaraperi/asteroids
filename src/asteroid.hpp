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

    void update(Graphics& g, float dt);
    void draw(Graphics& g) const;

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
    std::vector<Asteroid> split();

private:

    [[nodiscard]]
    std::vector<glm::vec2> init_asteroid_model();

private:
    Asteroid_Type _type;
    Transform _transform{};

    glm::vec2 _velocity{};
    float _angle_rotation_speed{};

    uint8_t _hp; // this many hits to destroy
    uint8_t _level_id;
    bool _dead{};

    std::vector<glm::vec2> _asteroid_model;
};
