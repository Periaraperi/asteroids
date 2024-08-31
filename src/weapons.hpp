#pragma once

#include "asteroid.hpp"
#include "bullet.hpp"
#include "physics.hpp"
#include "graphics.hpp"

class Shotgun {
public:
    void update(float dt);
    void shoot(const glm::vec2& pos, const glm::vec2& dir,
               std::vector<Bullet>& bullets);
    void reset();

    [[nodiscard]]
    float delay() const;

    [[nodiscard]]
    float timer() const;
private:
    float _delay{};
    float _timer{10.0f};
    glm::vec4 _color{0.8f, 0.6f, 0.7f, 1.0f};
};

class Gun {
public:
    void update(float dt);
    void shoot(const glm::vec2& pos, const glm::vec2& dir,
               std::vector<Bullet>& bullets);
    void reset();

    [[nodiscard]]
    float delay() const;
private:
    float _delay{};
    glm::vec4 _color{0.5f, 0.6f, 0.7f, 1.0f};
};

namespace Homing_Rocket {
    [[nodiscard]]
    inline int search(glm::vec2 ship_pos, const std::vector<Asteroid>& asteroids)
    {
        int index_to_target = 0;
        float current_min = std::numeric_limits<float>::max();
        for (int i{}; i<static_cast<int>(asteroids.size()); ++i) {
            const auto& a = asteroids[i];
            auto a_pos = a.get_world_pos();

            auto distance_squared = (a_pos.x-ship_pos.x)*(a_pos.x-ship_pos.x) + (a_pos.y-ship_pos.y)*(a_pos.y-ship_pos.y); 
            if (distance_squared < current_min) {
                index_to_target = i;
                current_min = distance_squared;
            }
        }

        return index_to_target;
    }
}

class Homing_Bullet {
public:
    Homing_Bullet() = default;

    // world_pos is center of square, 2*radius is side_length
    Homing_Bullet(glm::vec2 world_pos, int target_index, glm::vec2 dir, float radius, glm::vec4 color)
        :_pos{world_pos}, _target_index{target_index}, _dir_vector{dir}, _radius{radius},
        _color{color}, _dead{false}
    {}

    void update(float dt, glm::vec2 target)
    {
        auto w = 1600;
        auto h = 900;

        auto direction = target - _pos;
        direction = glm::normalize(direction);
        _desired_direction = direction;

        // stirring direction and rotation angle from cross product
        auto k = -(direction.x*_dir_vector.y - direction.y*_dir_vector.x);
        auto angle = 20.0f;
        auto dd = _dir_vector;
        if (k < 0) {
            _dir_vector.x = dd.x*std::cos(glm::radians(-angle)) - dd.y*std::sin(glm::radians(-angle));
            _dir_vector.y = dd.x*std::sin(glm::radians(-angle)) + dd.y*std::cos(glm::radians(-angle));
        }
        else {
            _dir_vector.x = dd.x*std::cos(glm::radians(angle)) - dd.y*std::sin(glm::radians(angle));
            _dir_vector.y = dd.x*std::sin(glm::radians(angle)) + dd.y*std::cos(glm::radians(angle));
        }
        std::cerr << k << '\n';

        // this later
        //_prev_pos = _pos;

        std::cerr << _dir_vector.x << " " << _dir_vector.y << '\n';
        _pos += _dir_vector*200.0f*dt;
        
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

    void set_pos(glm::vec2 pos)
    {_pos = pos;}

    void set_target(int target_index)
    { _target_index = target_index; }

    [[nodiscard]]
    int get_target() const
    { return _target_index; }

    void draw(Graphics& g, float alpha) const
    {
        g.draw_rect({_pos.x-_radius, _pos.y+_radius}, {2*_radius, 2*_radius}, _color);
        g.draw_line(_pos, _pos+_dir_vector*80.0f, {1.0f, 0.0f, 0.0f, 1.0f});
        g.draw_line(_pos, _pos+_desired_direction*80.0f, {1.0f, 1.0f, 0.0f, 1.0f});
        //g.draw_line(_pos+_dir_vector*80.0f, _pos+(_dir_vector+_change)*80.0f, {0.0f, 1.0f, 0.0f, 1.0f});
    }

    [[nodiscard]]
    std::vector<glm::vec2> get_world_points() const
    {
        return {
            {_pos.x-_radius, _pos.y+_radius},
            {_pos.x+_radius, _pos.y+_radius},
            {_pos.x+_radius, _pos.y-_radius},
            {_pos.x-_radius, _pos.y-_radius}
        };
    }

    [[nodiscard]]
    glm::vec2 get_world_pos() const 
    { return _pos; }

    [[nodiscard]]
    bool dead() const 
    { return _dead; }

    void explode()
    { _dead = true; }
private:
    glm::vec2 _pos;
    glm::vec2 _prev_pos;

    int _target_index;
    glm::vec2 _dir_vector;
    float _radius;
    glm::vec4 _color;
    bool _dead;
    glm::vec2 _desired_direction;
    glm::vec2 _change;

    float _timer{0.0f};
};
