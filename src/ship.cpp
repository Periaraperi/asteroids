#include "ship.hpp"

#include "graphics.hpp"
#include "input_manager.hpp"
#include "physics.hpp"

#include <algorithm>

#include <glm/gtc/matrix_transform.hpp>

constexpr float ROTATION_SPEED = 180.0f;
constexpr float SPEED = 400.0f;
float DECCELERATION_SPEED = 180.0f;
bool first_move = false;

std::vector<glm::vec2>
init_ship_model()
{
    return {{
        { 0.0f, -0.25f}, // 0
        {-0.5f, -0.5f},  // 1
        { 0.0f,  1.0f},  // 2
        { 0.5f, -0.5f}   // 3
    }};
}

Ship::Ship(glm::vec2 world_pos)
    :_ship_model{init_ship_model()},
     _transform{world_pos, {25.0f, 20.0f}, 0.0f},
     _velocity{0.0f, 0.0f}, _hp{3}, _iframe_duration{2.0f},
     _invincible{false}
{}

void Ship::update(Graphics& g, Input_Manager& im, float dt)
{
    _prev_transform = _transform;

    // ROTATION
    if (im.key_down(SDL_SCANCODE_A)) {
        _transform.angle += ROTATION_SPEED*dt;
        Transform::clamp_angle(_transform.angle);
    }
    if (im.key_down(SDL_SCANCODE_D)) {
        _transform.angle -= ROTATION_SPEED*dt;
        Transform::clamp_angle(_transform.angle);
    }

    if (im.key_down(SDL_SCANCODE_W)) {
        first_move = true;
        DECCELERATION_SPEED = 180.0f; // reset
        _velocity = get_direction_vector()*SPEED*dt;
        _transform.pos += _velocity;
    }
    else {
        if (first_move) {
            _velocity = get_direction_vector()*DECCELERATION_SPEED*dt;
            _transform.pos += _velocity;
            DECCELERATION_SPEED -= 7.0f*dt;
            if (DECCELERATION_SPEED < 0.0f) DECCELERATION_SPEED = 0.0f;
        }
    }

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

    if (_invincible) iframes(dt);
}

void Ship::iframes(float step)
{
    static float accum = 0.0f;

    //PERIA_LOG("DT: ", step);
    if (accum <= _iframe_duration) {
        //PERIA_LOG("Accum: ", accum);
        accum += step;
    }
    else {
        accum = 0.0f;
        _invincible = false;
    }
}

void Ship::hit()
{ 
    if (_hp > 0) --_hp;
    if (_hp > 0) _invincible = true;
}

uint8_t Ship::hp() const
{ return _hp; }

bool Ship::is_invincible() const
{ return _invincible; }

void Ship::draw(Graphics& g, float alpha) const
{ 
    auto t = interpolate_state(_prev_transform, _transform, alpha);
    if (_invincible) {
        g.draw_polygon(get_points_in_world_interpolated(t), {0.863f, 0.078f, 0.235f, 0.7f}); 
    }
    else {
        g.draw_polygon(get_points_in_world_interpolated(t), {0.55f, 0.3f, 0.8f, 1.0f}); 
    }
}

// returns world positions for polygon points
// for physics.
std::vector<glm::vec2> Ship::get_points_in_world() const
{
    std::vector<glm::vec2> vec; vec.reserve(_ship_model.size());
    auto transform = Transform::model(_transform.pos, _transform.scale, _transform.angle);
    for (const auto& p:_ship_model) {
        glm::vec4 v{p.x, p.y, 0.0f, 1.0f};
        glm::vec4 transformed = transform*v;
        vec.emplace_back(transformed.x, transformed.y);
    }
    return vec;
}

std::vector<glm::vec2> Ship::get_points_in_world_interpolated(const Transform& interpolated_transform) const
{
    std::vector<glm::vec2> vec; vec.reserve(_ship_model.size());
    auto model = Transform::model(interpolated_transform.pos, interpolated_transform.scale, _transform.angle);
    for (const auto& p:_ship_model) {
        glm::vec4 v{p.x, p.y, 0.0f, 1.0f};
        glm::vec4 transformed = model*v;
        vec.emplace_back(transformed.x, transformed.y);
    }
    return vec;
}

glm::vec2 Ship::get_direction_vector() const
{ return {std::cos(glm::radians(_transform.angle+90.0f)), std::sin(glm::radians(_transform.angle+90.0f))}; }
