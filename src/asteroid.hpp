#pragma once

#include <vector>
#include <glm/vec2.hpp>

class Graphics;

class Asteroid {
public:
    explicit Asteroid(Graphics& graphics);
    Asteroid(Graphics& graphics, glm::vec2 world_pos);

    void update(float dt);
    void draw();

    void set_velocity(glm::vec2 v);
    std::vector<glm::vec2> get_points_in_world();

private:
    std::vector<glm::vec2> gen_random_asteroid();
private:
    Graphics& _graphics;

    glm::vec2 _pos;
    glm::vec2 _velocity{};
    float _angle{};
    float _angle_rotation_speed{};

    std::vector<glm::vec2> _asteroid;
};
