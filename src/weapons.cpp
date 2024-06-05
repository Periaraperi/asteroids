#include "weapons.hpp"

#include <cmath>
#include <glm/trigonometric.hpp>

std::vector<Bullet> init_shotgun(glm::vec2 world_pos, glm::vec2 dir)
{
    auto angle1 = glm::radians(10.0f);
    auto angle2 = glm::radians(-10.0f);
    auto dir_left = glm::vec2{
        std::cos(angle1)*dir.x - std::sin(angle1)*dir.y,
        std::sin(angle1)*dir.x + std::cos(angle1)*dir.y,
    };

    auto dir_right = glm::vec2{
        std::cos(angle2)*dir.x - std::sin(angle2)*dir.y,
        std::sin(angle2)*dir.x + std::cos(angle2)*dir.y,
    };

    glm::vec4 color{0.8f, 0.6f, 0.7f, 1.0f};
    return {Bullet(world_pos, 5.0f, dir, color),
            Bullet(world_pos + dir*15.0f, 5.0f, dir, color), 
            Bullet(world_pos + dir*30.0f, 5.0f, dir, color), 
            Bullet(world_pos + dir_left*15.0f, 5.0f, dir_left, color), 
            Bullet(world_pos + dir_left*30.0f, 5.0f, dir_left, color), 
            Bullet(world_pos + dir_right*15.0f, 5.0f, dir_right, color), 
            Bullet(world_pos + dir_right*30.0f, 5.0f, dir_right, color)};
}

Shotgun::Shotgun(glm::vec2 world_pos, glm::vec2 dir)
    :_bullets{init_shotgun(world_pos, dir)}
{}

void Shotgun::update(float dt)
{
    for (std::size_t i{}; i<_bullets.size(); ++i) {
        _bullets[i].update(dt);
    }
}

void Shotgun::draw(Graphics& g, float alpha) const
{
    for (std::size_t i{}; i<_bullets.size(); ++i) {
        _bullets[i].draw(g, alpha);
    }
}

std::vector<Bullet>& Shotgun::get_bullets_world_points()
{ return _bullets; }

