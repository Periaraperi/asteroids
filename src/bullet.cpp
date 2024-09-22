#include "bullet.hpp"

#include "graphics.hpp"
#include "physics.hpp"
#include "game.hpp"

// in world space
constexpr float SPEED = 500.0f;

Bullet::Bullet(glm::vec2 world_pos, float radius, glm::vec2 dir, glm::vec4 color)
    :_pos{world_pos}, _radius{radius}, _dir_vector{dir}, _color{color}, _dead{false}
{}

void Bullet::update(float dt)
{
    _prev_pos = _pos;

    _pos += _dir_vector*SPEED*dt;

    const auto [w, h] = Game::get_world_size();
    if (_pos.x-_radius > w || _pos.x+_radius < 0.0f ||
        _pos.y-_radius > h || _pos.y+_radius < 0.0f) {
        _dead = true;
    }
}

void Bullet::draw(Graphics& g, float alpha) const
{
    glm::vec2 p{peria::lerp(_prev_pos.x, _pos.x, alpha), peria::lerp(_prev_pos.y, _pos.y, alpha)};
    g.draw_rect({p.x-_radius, p.y+_radius}, {2*_radius, 2*_radius}, _color);
}

void Bullet::explode()
{ _dead = true; }

// in clockwise order
std::vector<glm::vec2> Bullet::get_world_points() const
{
    return {
        {_pos.x-_radius, _pos.y+_radius},
        {_pos.x+_radius, _pos.y+_radius},
        {_pos.x+_radius, _pos.y-_radius},
        {_pos.x-_radius, _pos.y-_radius}
    };
}
