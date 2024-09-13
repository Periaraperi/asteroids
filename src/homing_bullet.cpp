#include "homing_bullet.hpp"

#include "graphics.hpp"

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

    auto k = (direction.x*_dir_vector.y - direction.y*_dir_vector.x);
    auto angle = glm::radians(15.0f);
    if (k > 0) {
        angle *= (-1);
    }
    auto x = _dir_vector.x;
    auto y = _dir_vector.y;
    _dir_vector = glm::vec2{
        std::cos(angle)*x - std::sin(angle)*y,
        std::sin(angle)*x + std::cos(angle)*y
    };

    // this later
    //_prev_pos = _pos;

    _pos += _dir_vector*250.0f*dt;
    
    if (_pos.x-_radius > w) {
        _pos.x -= (w+_radius);
    } 
    if (_pos.x+_radius < 0.0f) {
        _pos.x += (w+_radius);
    } 
    if (_pos.y-_radius > h) {
        _pos.y -= (h+_radius);
    } 
    if (_pos.y+_radius < 0.0f) {
        _pos.y += (h+_radius);
    }
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
    g.draw_rect({_pos.x-_radius, _pos.y+_radius}, {2*_radius, 2*_radius}, {1.0f, 0.5f, 0.2f, 1.0f});
}
