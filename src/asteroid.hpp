#pragma once

#include <vector>
#include <glm/vec2.hpp>

class Graphics;

class Asteroid {
public:
    //explicit Asteroid(Graphics& graphics);
    Asteroid(glm::vec2 world_pos);
    Asteroid(const std::vector<glm::vec2>& normalized_points, glm::vec2 world_pos);

    void update(Graphics& g, float dt);
    void draw(Graphics& g);

    glm::vec2 get_world_pos() const {return _pos;}
    void set_velocity(glm::vec2 v);
    std::vector<glm::vec2> get_points_in_world();
    std::pair<std::vector<glm::vec2>, std::vector<glm::vec2>> split();

private:
    std::vector<glm::vec2> gen_random_asteroid();

private:
    //Graphics& _graphics;

    glm::vec2 _pos;
    glm::vec2 _velocity{};
    float _angle{};
    float _angle_rotation_speed{};

    std::vector<glm::vec2> _asteroid;
};
