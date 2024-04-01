#include "asteroid.hpp"

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


Asteroid::Asteroid(Graphics& graphics)
    :_graphics{graphics},
     _asteroid{gen_random_asteroid()}
{
    gen_random_asteroid();
}

Asteroid::Asteroid(Graphics& graphics, glm::vec2 world_pos)
    :_graphics{graphics},
     _pos{world_pos},
     _asteroid{gen_random_asteroid()}
{
    _angle_rotation_speed = get_float(20.0f, 30.0f);
    gen_random_asteroid();
}

void Asteroid::update(float dt)
{
    _angle += _angle_rotation_speed*dt;
    clamp_angle(_angle);

    _pos.x += _velocity.x*dt;
    _pos.y += _velocity.y*dt;
}

glm::mat4 transform_mat(glm::vec2 pos, float scale, float angle=0.0f)
{
    return glm::translate(glm::mat4(1.0f), glm::vec3(pos.x, pos.y, 0.0f))*
           glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f))*
           glm::scale(glm::mat4(1.0f), glm::vec3(scale, scale, 1.0f));
}

void Asteroid::draw()
{
    _graphics.draw_polygon(_asteroid, transform_mat(_pos, 150.0f, _angle), {0.0f, 1.0f, 0.0f, 1.0f});
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

