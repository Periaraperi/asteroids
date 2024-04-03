#include "bullet.hpp"

#include "graphics.hpp"

// in world space
constexpr float RADIUS = 10.0f;
constexpr float SPEED = 600.0f;

Bullet::Bullet(glm::vec2 world_pos, glm::vec2 dir)
    :_pos{world_pos}, _dir_vector{dir}, _dead{false}
{}

void Bullet::update(Graphics& g, float dt)
{
    _pos += _dir_vector*SPEED*dt;

    auto [w, h] = g.get_window_size();
    if (_pos.x-RADIUS > w || _pos.x+RADIUS < 0.0f ||
        _pos.y-RADIUS > h || _pos.y+RADIUS < 0.0f) {
        _dead = true;
    }
}

void Bullet::draw(Graphics& g) const
{
    g.draw_circle(_pos, RADIUS, {0.5f, 0.5f, 0.0f, 1.0f});
}
