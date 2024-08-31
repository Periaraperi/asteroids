#include "weapons.hpp"

#include <cmath>
#include <glm/trigonometric.hpp>


// START SHOTGUN =========================================================================
// =======================================================================================
void Shotgun::update(float dt)
{
    _delay -= dt;
    _timer -= dt;
}

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

    bullets.push_back(Bullet(pos, 5.0f, dir, _color));
    bullets.push_back(Bullet(pos + dir*15.0f, 5.0f, dir, _color));
    bullets.push_back(Bullet(pos + dir*30.0f, 5.0f, dir, _color));
    bullets.push_back(Bullet(pos + dir_left*15.0f, 5.0f, dir_left, _color));
    bullets.push_back(Bullet(pos + dir_left*30.0f, 5.0f, dir_left, _color));
    bullets.push_back(Bullet(pos + dir_right*15.0f, 5.0f, dir_right, _color));
    bullets.push_back(Bullet(pos + dir_right*30.0f, 5.0f, dir_right, _color));

    _delay = 1.0f; // delay between fires
}

void Shotgun::reset()
{
    _delay = 0.0f; // if we take weapon can shoot right away instead of waiting 
    _timer = 10.0f;
}

float Shotgun::delay() const
{ return _delay; }

float Shotgun::timer() const
{ return _timer; }
// END SHOTGUN =========================================================================
// =====================================================================================

// START GUN ===========================================================================
// =====================================================================================
void Gun::update(float dt)
{ _delay -= dt; }

void Gun::reset()
{ _delay = 0.0f; }

void Gun::shoot(const glm::vec2& pos, const glm::vec2& dir,
                std::vector<Bullet>& bullets)
{ 
    bullets.emplace_back(pos, 4.5f, dir, _color); 
    _delay = 0.3f;
}

float Gun::delay() const
{ return _delay; }

// END GUN =============================================================================
// =====================================================================================

// START HOMING_ROCKET =================================================================
// =====================================================================================



// END HOMING_ROCKET ===================================================================
// =====================================================================================
