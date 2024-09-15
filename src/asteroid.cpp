#include "asteroid.hpp"

#include <algorithm>
#include <array>

#include "graphics.hpp"
#include "physics.hpp"
#include "peria_utils.hpp"

std::array<float, 3> get_speed {
    200.0f,
    150.0f,
    100.0f
};

std::vector<std::vector<glm::vec2>> predefined_models = {
    {{-0.35f, -0.25f}, {-0.55f, 0.25f}, {-0.27f, 0.75f}, {0.35f, 0.85f}, {0.45f, -0.30f}},
    {{-0.5f, -0.3f}, {-0.6f, 0.55f}, {0.0f, 0.75f}, {0.8f, 0.7f}, {0.8f, -0.35f}},
    {{0.0f, -0.6f}, {-0.8f, -0.2f}, {-0.6f, 0.8f}, {0.65f, 0.9f}, {0.75f, 0.55f}, {0.7f, -0.3f}},
    {{-0.6f, -0.15f}, {-0.65f, 0.1f}, {-0.2f, 0.85f}, {0.5f, 1.0f}, {0.9f, 0.4f}, {0.7f, -0.1f}, {0.2f, -0.3f}},
    {{-0.62f, 0.32f}, {-0.26f, 0.81f}, {0.63f, 0.52f}, {0.26f, 0.24f}, {0.45f, -0.42f}, {-0.63f, -0.20f}},
    {{-0.72f, 0.07f}, {-0.22f, 0.80f}, {-0.08f, 0.18f}, {0.44f, 0.60f}, {0.69f, -0.80f}, {-0.86f, -0.82f}, {-0.49f, -0.20f}}
};

std::vector<glm::vec2> Asteroid::init_asteroid_model()
{ return predefined_models[peria::get_int(0, predefined_models.size()-1)]; }

Asteroid::Asteroid(Asteroid_Type asteroid_type, glm::vec2 pos, glm::vec2 dir_vector,
                   uint8_t level_id)
    :_type{asteroid_type},
     _transform{pos, {}, 0.0f},
     _velocity{dir_vector},
     _angle_rotation_speed{peria::get_float(20.0f, 35.0f)},
     _level_id{level_id}, _dead{false}, 
     _asteroid_model{init_asteroid_model()}
{
    auto largest_hp = [this]() -> uint8_t {
        if (_level_id == 1) return 3;
        if (_level_id >= 2 && _level_id <= 3) return 4;
        if (_level_id >= 4 && _level_id <= 5) return 6;
        return 0;
    }();

    switch (_type) {
        case Asteroid_Type::SMALL:
            _transform.scale = {50.0f, 50.0f};
            _velocity *= get_speed[int(Asteroid_Type::SMALL)];
            _hp = largest_hp-2;
            break;
        case Asteroid_Type::MEDIUM:
            _transform.scale = {100.0f, 100.0f};
            _velocity *= get_speed[int(Asteroid_Type::MEDIUM)];
            _hp = largest_hp-1;
            break;
        case Asteroid_Type::LARGE:
            _transform.scale = {150.0f, 150.0f};
            _velocity *= get_speed[int(Asteroid_Type::LARGE)];
            _hp = largest_hp;
            break;
        default:
            _transform.scale = {};
    }
}

void Asteroid::update(Graphics& g, float dt)
{
    // store prev state
    _prev_transform = _transform;

    _transform.angle += _angle_rotation_speed*dt;
    Transform::clamp_angle(_transform.angle);

    _transform.pos += _velocity*dt;

    // screen wrap
    auto world_pos = get_points_in_world();
    auto min_x = std::min_element(world_pos.begin(), world_pos.end(), [](glm::vec2 a, glm::vec2 b) {return a.x<b.x;})->x;
    auto max_x = std::max_element(world_pos.begin(), world_pos.end(), [](glm::vec2 a, glm::vec2 b) {return a.x<b.x;})->x;
    auto min_y = std::min_element(world_pos.begin(), world_pos.end(), [](glm::vec2 a, glm::vec2 b) {return a.y<b.y;})->y;
    auto max_y = std::max_element(world_pos.begin(), world_pos.end(), [](glm::vec2 a, glm::vec2 b) {return a.y<b.y;})->y;
    
    //auto [sw, sh] = g.get_window_size();
    auto sw = 1600;
    auto sh = 900;

    bool wrap = false;

    if (min_x > sw) {
        _transform.pos.x -= (sw+max_x-min_x);
        wrap = true;
    }
    else if (max_x < 0.0f) {
        _transform.pos.x += (sw+max_x-min_x);
        wrap = true;
    }

    if (min_y > sh) {
        _transform.pos.y -= (sh+max_y-min_y);
        wrap = true;
    }
    else if (max_y < 0.0f) {
        _transform.pos.y += (sh+max_y-min_y);
        wrap = true;
    }

    if (wrap) _prev_transform = _transform;

    reset_color();
}

void Asteroid::draw(Graphics& g, float alpha) const
{ 
    //g.draw_polygon(get_points_in_world(), _color);
    auto t = interpolate_state(_prev_transform, _transform, alpha);
    g.draw_polygon(get_points_in_world_interpolated(t), _color); 

    g.draw_text(std::to_string(_hp), t.pos, {0.2f, 0.2f, 0.4f}, 0.5f);
}

void Asteroid::explode()
{ _dead = true; }

glm::vec2 Asteroid::get_world_pos() const
{ return _transform.pos; }

bool Asteroid::dead() const 
{ return _dead; }

uint8_t Asteroid::hp() const
{ return _hp; }

void Asteroid::hit()
{ if (_hp > 0) --_hp; }

std::vector<glm::vec2> Asteroid::get_points_in_world() const
{
    std::vector<glm::vec2> vec; vec.reserve(_asteroid_model.size());
    auto transform = Transform::model(_transform.pos, _transform.scale, _transform.angle);
    for (const auto& p:_asteroid_model) {
        glm::vec4 v{p.x, p.y, 0.0f, 1.0f};
        glm::vec4 transformed = transform*v;
        vec.emplace_back(transformed.x, transformed.y);
    }
    return vec;
}

std::vector<glm::vec2> Asteroid::get_points_in_world_interpolated(const Transform& interpolated_transform) const
{
    std::vector<glm::vec2> vec; vec.reserve(_asteroid_model.size());
    auto interpolated_model = Transform::model(interpolated_transform.pos, interpolated_transform.scale, _transform.angle);
    for (const auto& p:_asteroid_model) {
        glm::vec4 v{p.x, p.y, 0.0f, 1.0f};
        glm::vec4 transformed = interpolated_model*v;
        vec.emplace_back(transformed.x, transformed.y);
    }
    return vec;
}

std::vector<Asteroid> Asteroid::split()
{
    std::vector<Asteroid> asteroids;
    asteroids.reserve(6); // will spawn at most 6 asteroids

    switch(_type) {
        case Asteroid_Type::SMALL:
            break;
        case Asteroid_Type::MEDIUM:
            {
                auto angle = 360.0f / 6;
                for (std::size_t i{}; i<6; ++i) {
                    asteroids.emplace_back(Asteroid_Type::SMALL, _transform.pos, 
                            glm::vec2{std::cos(glm::radians(i*angle)), std::sin(glm::radians(i*angle))}, _level_id);
                }
                break;
            }
        case Asteroid_Type::LARGE:
            {
                auto angle = 360.0f / 3;
                for (std::size_t i{}; i<3; ++i) {
                    asteroids.emplace_back(Asteroid_Type::MEDIUM, _transform.pos, 
                            glm::vec2{std::cos(glm::radians(i*angle)), std::sin(glm::radians(i*angle))}, _level_id);
                }
                break;
            }
    }
    return asteroids;
}
