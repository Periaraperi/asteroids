#include "ship.hpp"

#include <algorithm>

#include <glm/gtc/matrix_transform.hpp>

#include "graphics.hpp"
#include "input_manager.hpp"

constexpr float ROTATION_SPEED = 180.0f;
constexpr float ACCELERATION = 100.0f;

static void clamp_angle(float& angle) noexcept
{
    static constexpr float FULL_ROT_ANGLE = 360.0f;
    if (angle >= FULL_ROT_ANGLE) {
        auto k {static_cast<int>(angle / FULL_ROT_ANGLE)};
        angle -= k*FULL_ROT_ANGLE;
    }
    else if (angle <= -FULL_ROT_ANGLE) {
        auto k {static_cast<int>(angle / (-FULL_ROT_ANGLE))};
        angle += k*FULL_ROT_ANGLE;
    }
}

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

Ship::Ship(Graphics& graphics, Input_Manager& input_manager)
    :_graphics{graphics},
     _input_manager{input_manager},
     _ship_model{init_ship_model()},
     _scale{50.0f}, _angle{0.0f}, _velocity{0.0f, 0.0f}
{
    auto [w, h] = _graphics.get_window_size();
    _pos = {w*0.5f, h*0.5f};
}

glm::mat4 Ship::transform_mat()
{
    return glm::translate(glm::mat4(1.0f), glm::vec3(_pos.x, _pos.y, 0.0f))*
           glm::rotate(glm::mat4(1.0f), glm::radians(_angle), glm::vec3(0.0f, 0.0f, 1.0f))*
           glm::scale(glm::mat4(1.0f), glm::vec3(_scale, _scale, 1.0f));
}

void Ship::update(float dt)
{
    // ROTATION
    if (_input_manager.key_down(SDL_SCANCODE_A)) {
        _angle += ROTATION_SPEED*dt;
        clamp_angle(_angle);
    }
    if (_input_manager.key_down(SDL_SCANCODE_D)) {
        _angle -= ROTATION_SPEED*dt;
        clamp_angle(_angle);
    }

    if (_input_manager.key_down(SDL_SCANCODE_W)) {
        // modify this later
        constexpr int SPEED = 450;
        _pos.x += std::cos(glm::radians(_angle+90.0f))*SPEED*dt;
        _pos.y += std::sin(glm::radians(_angle+90.0f))*SPEED*dt;
    }

    //_pos.x += _velocity.x*dt;
    //_pos.y += _velocity.y*dt;

    // screen wrap
    auto world_pos = get_points_in_world();
    auto min_x = std::min_element(world_pos.begin(), world_pos.end(), [](glm::vec2 a, glm::vec2 b) {return a.x<b.x;})->x;
    auto max_x = std::max_element(world_pos.begin(), world_pos.end(), [](glm::vec2 a, glm::vec2 b) {return a.x<b.x;})->x;
    auto min_y = std::min_element(world_pos.begin(), world_pos.end(), [](glm::vec2 a, glm::vec2 b) {return a.y<b.y;})->y;
    auto max_y = std::max_element(world_pos.begin(), world_pos.end(), [](glm::vec2 a, glm::vec2 b) {return a.y<b.y;})->y;
    
    auto [sw, sh] = _graphics.get_window_size();
    if (min_x > sw) {
        _pos.x -= (sw+max_x-min_x);
    }
    else if (max_x < 0.0f) {
        _pos.x += (sw+max_x-min_x);
    }

    if (min_y > sh) {
        _pos.y -= (sh+max_y-min_y);
    }
    else if (max_y < 0.0f) {
        _pos.y += (sh+max_y-min_y);
    }

}

void Ship::draw()
{
    _graphics.draw_polygon(_ship_model, transform_mat(), {1.0f, 0.0f, 0.0f, 1.0f});
}

std::vector<glm::vec2> Ship::get_points_in_world()
{
    std::vector<glm::vec2> vec; vec.reserve(_ship_model.size());
    auto transform = transform_mat();
    for (const auto& p:_ship_model) {
        glm::vec4 v{p.x, p.y, 0.0f, 1.0f};
        glm::vec4 transformed = transform*v;
        vec.emplace_back(transformed.x, transformed.y);
    }
    return vec;
}

