#include "weapons.hpp"

#include <cmath>
#include <glm/trigonometric.hpp>

// Delay - number of seconds per shots
// Timer - how many seconds a weapong can be used

// START GUN ===========================================================================
// =====================================================================================
void Gun::update(float dt)
{ _delay -= dt; }

void Gun::reset()
{ _delay = 0.0f; }

// default gun shoots 1 bullet at a time from tip of the ship with relatively small delay.
void Gun::shoot(const glm::vec2& pos, const glm::vec2& dir,
                std::vector<Bullet>& bullets) 
{ 
    bullets.emplace_back(pos, 4.5f, dir, _bullet_color); 
    _delay = Gun::_initial_delay;
}

float Gun::delay() const
{ return _delay; }

void Gun::set_initial_delay(const float delay)
{ _initial_delay = delay; }
// END GUN =============================================================================
// =====================================================================================

// START SHOTGUN =========================================================================
// =======================================================================================
void Shotgun::update(float dt)
{
    _delay -= dt;
    _timer -= dt;
}

// shotgun shoots several bullets in cone shape from tip of ship with relatively large delay
void Shotgun::shoot(const glm::vec2& pos, const glm::vec2& dir,
                    std::vector<Bullet>& bullets)
{ 
    auto angle1 = glm::radians(10.0f);
    auto angle2 = glm::radians(-10.0f);
    auto dir_left = glm::vec2{
        std::cos(angle1)*dir.x - std::sin(angle1)*dir.y,
        std::sin(angle1)*dir.x + std::cos(angle1)*dir.y,
    };

    auto dir_right = glm::vec2{
        std::cos(angle2)*dir.x - std::sin(angle2)*dir.y,
        std::sin(angle2)*dir.x + std::cos(angle2)*dir.y,
    };

    bullets.push_back(Bullet(pos, 5.0f, dir, _bullet_color));
    bullets.push_back(Bullet(pos + dir_left*15.0f, 5.0f, dir_left, _bullet_color));
    bullets.push_back(Bullet(pos + dir_right*15.0f, 5.0f, dir_right, _bullet_color));
    
    if (_upgrade_level >= 1) {
        bullets.push_back(Bullet(pos + dir_right*30.0f, 5.0f, dir_right, _bullet_color));
        bullets.push_back(Bullet(pos + dir_left*30.0f, 5.0f, dir_left, _bullet_color));
    }
    if (_upgrade_level >= 2) {
        bullets.push_back(Bullet(pos + dir*30.0f, 5.0f, dir, _bullet_color));
        bullets.push_back(Bullet(pos + dir*15.0f, 5.0f, dir, _bullet_color));
    }
    if (_upgrade_level >= 3) {
        bullets.push_back(Bullet(pos + dir_right*45.0f, 5.0f, dir_right, _bullet_color));
        bullets.push_back(Bullet(pos + dir_left*45.0f, 5.0f, dir_left, _bullet_color));
    }

    _delay = Shotgun::_initial_delay;
}

void Shotgun::reset()
{
    _delay = 0.0f; 
    _timer = Shotgun::_initial_timer;
}

void Shotgun::upgrade()
{ _upgrade_level = std::min(_upgrade_level+1, 3); }

float Shotgun::delay() const
{ return _delay; }

float Shotgun::timer() const
{ return _timer; }
// END SHOTGUN =========================================================================
// =====================================================================================

// START HOMING_ROCKET =================================================================
// =====================================================================================

int Homing_Gun::search(glm::vec2 ship_pos, const std::vector<Asteroid>& asteroids) 
{
    int index_to_target = -1;
    float current_min = std::numeric_limits<float>::max();
    auto asteroids_len = static_cast<int>(asteroids.size());
    for (int i{}; i<asteroids_len; ++i) {
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

void Homing_Gun::update(float dt)
{
    _delay -= dt;
    _timer -= dt;
}

void Homing_Gun::reset()
{
    _delay = 0.0f;
    _timer = Homing_Gun::_initial_timer;
}

void Homing_Gun::do_delay()
{ _delay = _initial_delay; }

void Homing_Gun::set_delay(float d)
{ _initial_delay = d; }

float Homing_Gun::delay() const
{ return _delay; }

float Homing_Gun::timer() const
{ return _timer; }
// END HOMING_ROCKET ===================================================================
// =====================================================================================
