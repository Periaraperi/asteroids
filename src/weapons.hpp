#pragma once

#include "bullet.hpp"

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
