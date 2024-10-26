#include "asteroid.hpp"

#include <algorithm>
#include <array>

#include "graphics.hpp"
#include "physics.hpp"
#include "game.hpp"
#include "peria_utils.hpp"

std::array<float, 3> get_speed {
    250.0f,
    200.0f,
    150.0f
};

// made in helper editor
std::vector<std::vector<glm::vec2>> predefined_models = {
    {{-0.429375f, 0.0588889f}, {-0.3575f, 0.342222f}, {-0.15f, 0.447778f}, {0.095625f, 0.392222f}, {0.29375f, 0.161111f}, {0.37625f, -0.195556f}, {0.238125f, -0.423333f}, {-0.245f, -0.34f}},
    {{-0.478125f, 0.0522222f}, {-0.475625f, 0.308889f}, {-0.3175f, 0.474444f}, {-0.17125f, 0.397778f}, {0.04625f, 0.474444f}, {0.144375f, 0.295556f}, {0.340625f, 0.0911111f}, {0.27625f, -0.0477778f}, {0.3675f, -0.295556f}, {0.286875f, -0.462222f}, {0.1275f, -0.466667f}, {-0.04625f, -0.331111f}, {-0.2175f, -0.367778f}, {-0.4625f, -0.317778f}},
};
std::vector<std::vector<glm::vec2>> predefined_models_medium {
    {{-0.399375f, 0.124444f}, {-0.2925f, 0.39f}, {-0.1025f, 0.465556f}, {0.05f, 0.38f}, {0.19375f, 0.451111f}, {0.241875f, 0.288889f}, {0.145f, 0.166667f}, {0.165f, 0.0411111f}, {0.325f, 0.0666667f}, {0.340625f, -0.142222f}, {0.146875f, -0.358889f}, {-0.04625f, -0.44f}, {-0.166875f, -0.264444f}, {-0.328125f, -0.265556f}, {-0.28f, -0.0822222f}, {-0.301875f, 0.0444444f}},
    {{-0.47375f, 0.0333333f}, {-0.4625f, 0.368889f}, {-0.270625f, 0.475556f}, {-0.0825f, 0.427778f}, {0.254375f, 0.456667f}, {0.431875f, 0.318889f}, {0.310625f, 0.174444f}, {0.291875f, -0.0511111f}, {0.40125f, -0.19f}, {0.305f, -0.486667f}, {0.03625f, -0.365556f}, {-0.065f, -0.447778f}, {-0.265f, -0.261111f}, {-0.443125f, -0.255556f}},
};
std::vector<std::vector<glm::vec2>> predefined_models_small {
    {{-0.450625f, 0.136667f}, {-0.356875f, 0.39f}, {-0.251875f, 0.39f}, {-0.21875f, 0.234444f}, {-0.10375f, 0.118889f}, {-0.010625f, 0.365556f}, {0.140625f, 0.438889f}, {0.25125f, 0.233333f}, {0.175625f, -0.0644444f}, {0.254375f, -0.262222f}, {0.064375f, -0.457778f}, {-0.1925f, -0.265556f}, {-0.464375f, -0.352222f}, {-0.385f, -0.104444f}},
    {{-0.444375f, 0.373333f}, {-0.2075f, 0.478889f}, {0.36625f, 0.274444f}, {0.37375f, -0.197778f}, {0.20125f, -0.0744444f}, {0.1675f, -0.454444f}, {-0.0525f, -0.197778f}, {-0.2225f, -0.196667f}, {-0.47875f, -0.44f}, {-0.4375f, -0.01f}},
};

std::vector<glm::vec2> Asteroid::init_asteroid_model(Asteroid_Type type)
{
    if (type == Asteroid_Type::LARGE)       return predefined_models[peria::get_int(0, predefined_models.size()-1)];
    else if (type == Asteroid_Type::MEDIUM) return predefined_models_medium[peria::get_int(0, predefined_models_medium.size()-1)];
    else                                    return predefined_models_small[peria::get_int(0, predefined_models_small.size()-1)];
}

Asteroid::Asteroid(Asteroid_Type asteroid_type, glm::vec2 pos, glm::vec2 dir_vector, uint8_t level_id)
    :_type{asteroid_type},
     _transform{pos, {}, 0.0f}, 
     _prev_transform{_transform},
     _velocity{dir_vector},
     _angle_rotation_speed{peria::get_float(20.0f, 35.0f)},
     _level_id{level_id}, _dead{false},
     _asteroid_model{init_asteroid_model(asteroid_type)}
{
    auto largest_hp = [this]() -> uint8_t {
        if (_level_id == 1) return 3;
        if (_level_id == 2) return 4;
        if (_level_id == 3) return 5;
        if (_level_id == 4) return 3;
        if (_level_id == 5) return 4;
        return 0;
    }();

    auto random_speed_offset = peria::get_int(-20, 20);
    switch (_type) {
        case Asteroid_Type::SMALL:
            _transform.scale = {70.0f, 70.0f};
            _velocity *= get_speed[int(Asteroid_Type::SMALL)]+random_speed_offset;
            _hp = largest_hp-2;
            break;
        case Asteroid_Type::MEDIUM:
            _transform.scale = {180.0f, 180.0f};
            _velocity *= get_speed[int(Asteroid_Type::MEDIUM)] + random_speed_offset;
            _hp = largest_hp-1;
            break;
        case Asteroid_Type::LARGE:
            _transform.scale = {250.0f, 250.0f};
            _velocity *= get_speed[int(Asteroid_Type::LARGE)] + random_speed_offset;
            _hp = largest_hp;
            break;
        default:
            _transform.scale = {};
    }
}

void Asteroid::update(float dt)
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
    
    const auto [w, h] = Game::get_world_size();

    bool wrap = false;

    if (min_x > w) {
        _transform.pos.x -= (w+max_x-min_x);
        wrap = true;
    }
    else if (max_x < 0.0f) {
        _transform.pos.x += (w+max_x-min_x);
        wrap = true;
    }

    if (min_y > h) {
        _transform.pos.y -= (h+max_y-min_y);
        wrap = true;
    }
    else if (max_y < 0.0f) {
        _transform.pos.y += (h+max_y-min_y);
        wrap = true;
    }

    if (wrap) _prev_transform = _transform;

    reset_color();
}

void Asteroid::draw(Graphics& g, float alpha) const
{ 
    //g.draw_polygon(get_points_in_world(), _color);
    auto t = peria::interpolate_state(_prev_transform, _transform, alpha);
    g.draw_polygon(get_points_in_world_interpolated(t), _color); 

    g.draw_text(std::to_string(_hp), t.pos, {0.2f, 0.2f, 0.4f}, 48, 0.5f);
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
                    auto direction = glm::vec2{std::cos(glm::radians(i*angle)), std::sin(glm::radians(i*angle))};
                    auto offset = direction*10.0f;
                    asteroids.emplace_back(Asteroid_Type::SMALL, _transform.pos+offset, 
                            direction, _level_id);
                }
                break;
            }
        case Asteroid_Type::LARGE:
            {
                auto angle = 360.0f / 3;
                for (std::size_t i{}; i<3; ++i) {
                    auto direction = glm::vec2{std::cos(glm::radians(i*angle)), std::sin(glm::radians(i*angle))};
                    auto offset = direction*10.0f;
                    asteroids.emplace_back(Asteroid_Type::MEDIUM, _transform.pos+offset, 
                            direction, _level_id);
                }
                break;
            }
    }
    return asteroids;
}
