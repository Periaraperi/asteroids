#pragma once

#include "bullet.hpp"

class Shotgun {
public:
    Shotgun() = default;
    // dir is vector pointing to middle bullet line
    Shotgun(glm::vec2 world_pos, glm::vec2 dir);

    void update(float dt);
    void draw(Graphics& g, float alpha) const;
    
    [[nodiscard]]
    std::vector<Bullet>& get_bullets_world_points();

private:
    std::vector<Bullet> _bullets;
};
