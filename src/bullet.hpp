#pragma once

#include <glm/vec2.hpp>
#include <vector>

class Graphics;

class Bullet {
public:
    Bullet() = default;
    Bullet(glm::vec2 world_pos, glm::vec2 dir);
    void update(Graphics& g, float dt);
    void draw(Graphics& g, float alpha) const;

    [[nodiscard]]
    std::vector<glm::vec2> get_world_points() const;

    [[nodiscard]]
    glm::vec2 get_world_pos() const 
    { return _pos; }

    [[nodiscard]]
    bool dead() const 
    { return _dead; }

    void explode();

private:
    glm::vec2 _pos;
    glm::vec2 _prev_pos;

    glm::vec2 _dir_vector;
    bool _dead;
};
