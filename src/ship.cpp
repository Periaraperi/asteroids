#include "ship.hpp"

#include "graphics.hpp"
#include "input_manager.hpp"

#include <algorithm>

#include <glm/gtc/matrix_transform.hpp>

constexpr float ROTATION_SPEED = 180.0f;
constexpr float ACCELERATION = 100.0f;

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
     _transform{world_pos, {50.0f, 50.0f}, 0.0f},
     _velocity{0.0f, 0.0f}
{}

void Ship::update(Graphics& g, Input_Manager& im, float dt)
{
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
        // modify this later
        constexpr int SPEED = 450;
        _transform.pos.x += std::cos(glm::radians(_transform.angle+90.0f))*SPEED*dt;
        _transform.pos.y += std::sin(glm::radians(_transform.angle+90.0f))*SPEED*dt;
    }

    //_pos.x += _velocity.x*dt;
    //_pos.y += _velocity.y*dt;

    // screen wrap
    auto world_pos = get_points_in_world();
    auto min_x = std::min_element(world_pos.begin(), world_pos.end(), [](glm::vec2 a, glm::vec2 b) {return a.x<b.x;})->x;
    auto max_x = std::max_element(world_pos.begin(), world_pos.end(), [](glm::vec2 a, glm::vec2 b) {return a.x<b.x;})->x;
    auto min_y = std::min_element(world_pos.begin(), world_pos.end(), [](glm::vec2 a, glm::vec2 b) {return a.y<b.y;})->y;
    auto max_y = std::max_element(world_pos.begin(), world_pos.end(), [](glm::vec2 a, glm::vec2 b) {return a.y<b.y;})->y;
    
    auto [sw, sh] = g.get_window_size();
    if (min_x > sw) {
        _transform.pos.x -= (sw+max_x-min_x);
    }
    else if (max_x < 0.0f) {
        _transform.pos.x += (sw+max_x-min_x);
    }

    if (min_y > sh) {
        _transform.pos.y -= (sh+max_y-min_y);
    }
    else if (max_y < 0.0f) {
        _transform.pos.y += (sh+max_y-min_y);
    }

}

void Ship::draw(Graphics& g) const
{
    g.draw_polygon(get_points_in_world(), {1.0f, 0.0f, 0.0f, 1.0f});
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

glm::vec2 Ship::get_direction_vector() const
{
    return {std::cos(glm::radians(_transform.angle+90.0f)), std::sin(glm::radians(_transform.angle+90.0f))};
}
