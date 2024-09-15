#include "homing_bullet.hpp"

#include "graphics.hpp"
#include "physics.hpp"

static constexpr float ROT_SPEED = 100.0f;
static constexpr float BULLET_SPEED = 350.0f;

Homing_Bullet::Homing_Bullet(glm::vec2 world_pos, float radius, int target_index, glm::vec2 initial_direction, glm::vec4 color)
    :_pos{world_pos}, 
    _dir_vector{initial_direction}, 
    _color{color},
    _radius{radius},
    _target_index{target_index}, 
    _dead{false}
{}

void Homing_Bullet::update(float dt, glm::vec2 target)
{
    auto w = 1600;
    auto h = 900;

    auto direction = target - _pos;
    direction = glm::normalize(direction);

    // cross product to tell on which side is target
    auto k = (direction.x*_dir_vector.y - direction.y*_dir_vector.x);
    auto angle_delta= glm::radians((-k)*ROT_SPEED*dt);

    auto x = _dir_vector.x;
    auto y = _dir_vector.y;
    _dir_vector = glm::vec2{
        std::cos(angle_delta)*x - std::sin(angle_delta)*y,
        std::sin(angle_delta)*x + std::cos(angle_delta)*y
    };

    _prev_pos = _pos;

    _pos += _dir_vector*BULLET_SPEED*dt;
    
    bool wrap = false;
    if (_pos.x-_radius > w) {
        _pos.x -= (w+_radius);
        wrap = true;
    } 
    if (_pos.x+_radius < 0.0f) {
        _pos.x += (w+_radius);
        wrap = true;
    } 
    if (_pos.y-_radius > h) {
        _pos.y -= (h+_radius);
        wrap = true;
    } 
    if (_pos.y+_radius < 0.0f) {
        _pos.y += (h+_radius);
        wrap = true;
    }
    if (wrap) _prev_pos = _pos;
}

void Homing_Bullet::set_target(int target_index)
{ _target_index = target_index; }

int Homing_Bullet::get_target() const
{ return _target_index; }

std::vector<glm::vec2> Homing_Bullet::get_world_points() const
{
    return {
        {_pos.x-_radius, _pos.y+_radius},
        {_pos.x+_radius, _pos.y+_radius},
        {_pos.x+_radius, _pos.y-_radius},
        {_pos.x-_radius, _pos.y-_radius}
    };
}

glm::vec2 Homing_Bullet::get_world_pos() const 
{ return _pos; }

bool Homing_Bullet::dead() const 
{ return _dead; }

void Homing_Bullet::explode()
{ _dead = true; }

void Homing_Bullet::draw(Graphics& g, float alpha) const
{
    glm::vec2 p{lerp(_prev_pos.x, _pos.x, alpha), lerp(_prev_pos.y, _pos.y, alpha)};
    g.draw_rect({_pos.x-_radius, _pos.y+_radius}, {2*_radius, 2*_radius}, {1.0f, 0.5f, 0.2f, 1.0f});
}
