#include "bullet.hpp"

#include "graphics.hpp"
#include "physics.hpp"

// in world space
constexpr float RADIUS = 4.5f;
constexpr float SPEED = 500.0f;

Bullet::Bullet(glm::vec2 world_pos, glm::vec2 dir)
    :_pos{world_pos}, _dir_vector{dir}, _dead{false}
{}

void Bullet::update(float dt)
{
    _prev_pos = _pos;

    _pos += _dir_vector*SPEED*dt;

    //auto [w, h] = g.get_window_size();
    auto w = 1600;
    auto h = 900;
    if (_pos.x-RADIUS > w || _pos.x+RADIUS < 0.0f ||
        _pos.y-RADIUS > h || _pos.y+RADIUS < 0.0f) {
        _dead = true;
    }
}

void Bullet::draw(Graphics& g, float alpha) const
{
    glm::vec2 p{lerp(_prev_pos.x, _pos.x, alpha), lerp(_prev_pos.y, _pos.y, alpha)};
    g.draw_rect({p.x-RADIUS, p.y+RADIUS}, {2*RADIUS, 2*RADIUS}, {0.5f, 0.6f, 0.7f, 1.0f});
    //g.draw_circle({_pos.x-RADIUS, _pos.y+RADIUS}, RADIUS, {0.5f, 0.5f, 0.0f, 1.0f});
}

void Bullet::explode()
{ _dead = true; }

// in clockwise order
std::vector<glm::vec2> Bullet::get_world_points() const
{
    return {
        {_pos.x-RADIUS, _pos.y+RADIUS},
        {_pos.x+RADIUS, _pos.y+RADIUS},
        {_pos.x+RADIUS, _pos.y-RADIUS},
        {_pos.x-RADIUS, _pos.y-RADIUS}
    };
}
