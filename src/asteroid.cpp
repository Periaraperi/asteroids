#include "asteroid.hpp"

#include <algorithm>
#include <random>

#include "graphics.hpp"
#include "peria_logger.hpp"

std::random_device rd = std::random_device();
std::mt19937 generator(rd());

[[nodiscard]]
float get_float(float l, float r)
{
    std::uniform_real_distribution<float> dist(l, r);
    return dist(rd);
}

[[nodiscard]]
int get_int(int l, int r)
{
    std::uniform_int_distribution<> dist(l, r);
    return dist(rd);
}

Asteroid::Asteroid(glm::vec2 world_pos)
    :_transform{world_pos, {150.0f, 150.0f}, 0.0f},
     _angle_rotation_speed{get_float(20.0f, 30.0f)},
     _dead{false},
     _asteroid_model{gen_random_asteroid()}
{}

Asteroid::Asteroid(const std::vector<glm::vec2>& normalized_points, glm::vec2 world_pos)
    :_transform{world_pos, {150.0f, 150.0f}, 0.0f},
     _angle_rotation_speed{get_float(20.0f, 30.0f)},
     _dead{false},
     _asteroid_model{normalized_points}
{}

void Asteroid::update(Graphics& g, float dt)
{
    _transform.angle += _angle_rotation_speed*dt;
    Transform::clamp_angle(_transform.angle);

    _transform.pos.x += _velocity.x*dt;
    _transform.pos.y += _velocity.y*dt;

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

void Asteroid::draw(Graphics& g)
{
    g.draw_polygon(_asteroid_model, Transform::model(_transform.pos, _transform.scale, _transform.angle), {0.0f, 1.0f, 0.0f, 1.0f});
}

// will work for now
[[nodiscard]]
std::vector<glm::vec2> Asteroid::gen_random_asteroid()
{
    // start with regular n sided polygon
    // and then randomly move points by small angle
    auto n = get_int(4, 7);
    std::vector<glm::vec2> a(n);
    float angle = 360.0f / n;

    for (std::size_t i{}; i<a.size(); ++i) {
        a[i] = {std::cos(glm::radians(angle*i)), std::sin(glm::radians(angle*i))};
    }
    
    for (auto& i:a) {
        int k = get_int(0,1);
        if (k) {
            float theta = glm::radians(get_float(10.0f, 50.0f));
            glm::vec2 v = i;
            i.x = std::cos(theta)*v.x - std::sin(theta)*v.y;
            i.y = std::sin(theta)*v.x + std::cos(theta)*v.y;
        }
    }

    return a;
}

void Asteroid::set_velocity(glm::vec2 v)
{ _velocity = v; }

void Asteroid::explode()
{ _dead = true; }

[[nodiscard]]
std::vector<glm::vec2> Asteroid::get_points_in_world()
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

std::pair<std::vector<glm::vec2>, std::vector<glm::vec2>> Asteroid::split()
{
    if (_asteroid_model.size() == 3) {
        return {{}, {}};
    }

    int a = get_int(0, _asteroid_model.size()-1);
    int b = (a+2) % _asteroid_model.size();
    
    std::vector<glm::vec2> asteroid1;
    for (int i=a; i!=b; i=((i+1)%_asteroid_model.size())) {
        asteroid1.push_back(_asteroid_model[i]);
    }
    asteroid1.push_back(_asteroid_model[b]);

    std::vector<glm::vec2> asteroid2;
    for(int i=b; i!=a; i=((i+1)%_asteroid_model.size())) {
        asteroid2.push_back(_asteroid_model[i]);
    }
    asteroid2.push_back(_asteroid_model[a]);

    return {asteroid1, asteroid2};
}
