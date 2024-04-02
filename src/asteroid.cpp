#include "asteroid.hpp"

#include <algorithm>
#include <random>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "graphics.hpp"
#include "peria_logger.hpp"

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

std::random_device rd = std::random_device();
std::mt19937 generator(rd());

float get_float(float l, float r)
{
    std::uniform_real_distribution<float> dist(l, r);
    return dist(rd);
}

int get_int(int l, int r)
{
    std::uniform_int_distribution<> dist(l, r);
    return dist(rd);
}

//Asteroid::Asteroid(Graphics& graphics)
//    :_graphics{graphics},
//     _asteroid{gen_random_asteroid()}
//{
//    gen_random_asteroid();
//}
    
Asteroid::Asteroid(const std::vector<glm::vec2>& normalized_points, glm::vec2 world_pos)
    :_pos{world_pos}, _asteroid{normalized_points}
{
    _angle_rotation_speed = get_float(20.0f, 30.0f);
}

Asteroid::Asteroid(glm::vec2 world_pos)
    :_pos{world_pos},
     _asteroid{gen_random_asteroid()}
{
    _angle_rotation_speed = get_float(20.0f, 30.0f);
    gen_random_asteroid();
}

void Asteroid::update(Graphics& g, float dt)
{
    _angle += _angle_rotation_speed*dt;
    clamp_angle(_angle);

    _pos.x += _velocity.x*dt;
    _pos.y += _velocity.y*dt;

    // screen wrap
    auto world_pos = get_points_in_world();
    auto min_x = std::min_element(world_pos.begin(), world_pos.end(), [](glm::vec2 a, glm::vec2 b) {return a.x<b.x;})->x;
    auto max_x = std::max_element(world_pos.begin(), world_pos.end(), [](glm::vec2 a, glm::vec2 b) {return a.x<b.x;})->x;
    auto min_y = std::min_element(world_pos.begin(), world_pos.end(), [](glm::vec2 a, glm::vec2 b) {return a.y<b.y;})->y;
    auto max_y = std::max_element(world_pos.begin(), world_pos.end(), [](glm::vec2 a, glm::vec2 b) {return a.y<b.y;})->y;
    
    auto [sw, sh] = g.get_window_size();
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

glm::mat4 transform_mat(glm::vec2 pos, float scale, float angle=0.0f)
{
    return glm::translate(glm::mat4(1.0f), glm::vec3(pos.x, pos.y, 0.0f))*
           glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f))*
           glm::scale(glm::mat4(1.0f), glm::vec3(scale, scale, 1.0f));
}

void Asteroid::draw(Graphics& g)
{
    g.draw_polygon(_asteroid, transform_mat(_pos, 150.0f, _angle), {0.0f, 1.0f, 0.0f, 1.0f});
}

// will work for now
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

std::vector<glm::vec2> Asteroid::get_points_in_world()
{
    std::vector<glm::vec2> vec; vec.reserve(_asteroid.size());
    auto transform = transform_mat(_pos, 150.0f, _angle);
    for (const auto& p:_asteroid) {
        glm::vec4 v{p.x, p.y, 0.0f, 1.0f};
        glm::vec4 transformed = transform*v;
        vec.emplace_back(transformed.x, transformed.y);
    }
    return vec;
}

std::pair<std::vector<glm::vec2>, std::vector<glm::vec2>> Asteroid::split()
{
    if (_asteroid.size() == 3) {
        return {{}, {}};
    }

    int a = get_int(0, _asteroid.size()-1);
    int b = (a+2) % _asteroid.size();
    
    std::vector<glm::vec2> asteroid1;
    for (int i=a; i!=b; i=((i+1)%_asteroid.size())) {
        asteroid1.push_back(_asteroid[i]);
    }
    asteroid1.push_back(_asteroid[b]);

    std::vector<glm::vec2> asteroid2;
    for(int i=b; i!=a; i=((i+1)%_asteroid.size())) {
        asteroid2.push_back(_asteroid[i]);
    }
    asteroid2.push_back(_asteroid[a]);

    return {asteroid1, asteroid2};
}
