#pragma once

#include "asteroid.hpp"
#include "bullet.hpp"

class Gun {
public:
    void update(float dt);
    void shoot(const glm::vec2& pos, const glm::vec2& dir,
               std::vector<Bullet>& bullets);
    void reset();

    [[nodiscard]]
    float delay() const;

    void set_initial_delay(const float delay);
private:
    float _delay{};
    glm::vec4 _bullet_color{0.5f, 0.6f, 0.7f, 1.0f};

    float _initial_delay{0.4f};
};

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
    glm::vec4 _bullet_color{0.8f, 0.6f, 0.7f, 1.0f};

    static constexpr float _initial_delay{1.0f};
    static constexpr float _initial_timer{10.0f};
};

class Homing_Gun {
public:
    [[nodiscard]]
    int search(glm::vec2 ship_pos, const std::vector<Asteroid>& asteroids);

    void update(float dt);
    void reset();

    void draw_radar(Graphics& g, glm::vec2 pos);

    [[nodiscard]]
    float delay() const;

    [[nodiscard]]
    float timer() const;
private:
    float _delay{0.0f};
    float _timer{10.0f};

    static constexpr float _initial_delay{0.5f};
    static constexpr float _initial_timer{10.0f};
};
