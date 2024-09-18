#include "homing_bullet.hpp"

#include "graphics.hpp"
#include "physics.hpp"

static constexpr float ROT_SPEED = 100.0f;
static constexpr float BULLET_SPEED = 350.0f;

Homing_Bullet::Homing_Bullet(glm::vec2 world_pos, float radius, int target_index, glm::vec2 initial_direction, float initial_angle, glm::vec4 color)
    :_transform{world_pos, {radius*2.0f, radius*2.0f}, initial_angle},
    _prev_transform{_transform},
    _dir_vector{initial_direction}, 
    _color{color},
    _target_index{target_index}, 
    _dead{false}
{}

void Homing_Bullet::update(float dt)
{
    auto w = 1600;
    auto h = 900;

    // homing logic
    if (_target_index != -1) { 
        auto direction = _target_pos - _transform.pos;
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
    }

    // if not targeting any more still continue flying 

    _prev_transform = _transform;

    _transform.pos += _dir_vector*BULLET_SPEED*dt;
    
    bool wrap = false;
    auto& pos = _transform.pos;
    const auto& radius = _transform.scale.x*0.5f;
    if (pos.x-radius > w) {
        pos.x -= (w+radius);
        wrap = true;
    } 
    if (pos.x+radius < 0.0f) {
        pos.x += (w+radius);
        wrap = true;
    } 
    if (pos.y-radius > h) {
        pos.y -= (h+radius);
        wrap = true;
    } 
    if (pos.y+radius < 0.0f) {
        pos.y += (h+radius);
        wrap = true;
    }
    if (wrap) _prev_transform = _transform;
}

void Homing_Bullet::set_target_index(int target_index)
{ _target_index = target_index; }

int Homing_Bullet::get_target_index() const
{ return _target_index; }

void Homing_Bullet::set_target_pos(glm::vec2 target_pos)
{ _target_pos = target_pos; }

std::vector<glm::vec2> Homing_Bullet::get_world_points() const
{
    const auto& pos = _transform.pos;
    const auto& radius = _transform.scale.x*0.5f;
    return {
        {pos.x-radius, pos.y+radius},
        {pos.x+radius, pos.y+radius},
        {pos.x+radius, pos.y-radius},
        {pos.x-radius, pos.y-radius}
    };
}

glm::vec2 Homing_Bullet::get_world_pos() const 
{ return _transform.pos; }

bool Homing_Bullet::dead() const 
{ return _dead; }

void Homing_Bullet::explode()
{ _dead = true; }

void Homing_Bullet::draw(Graphics& g, float alpha) const
{
    auto t = peria::interpolate_state(_prev_transform, _transform, alpha);
    g.draw_rect({t.pos.x-t.scale.x*0.5f, t.pos.y-t.scale.y*0.5f}, t.scale, {1.0f, 0.5f, 0.2f, 1.0f});
}
