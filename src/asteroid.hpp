#pragma once

#include <vector>
#include "transform.hpp"

class Graphics;

class Asteroid {
public:
    enum class Asteroid_Type {
        DEFAULT = 0
    };

    explicit Asteroid(glm::vec2 world_pos);
    Asteroid(const std::vector<glm::vec2>& normalized_points, glm::vec2 world_pos);

    void update(Graphics& g, float dt);
    void draw(Graphics& g);

    glm::vec2 get_world_pos() const {return _transform.pos;}
    bool dead() const { return _dead; }
    void explode();
    
    void set_velocity(glm::vec2 v);

    std::vector<glm::vec2> get_points_in_world();

    std::pair<std::vector<glm::vec2>, std::vector<glm::vec2>> split();

private:
    std::vector<glm::vec2> gen_random_asteroid();

private:
    Transform _transform{};

    glm::vec2 _velocity{};
    float _angle_rotation_speed{};
    bool _dead{};

    std::vector<glm::vec2> _asteroid_model;
};
