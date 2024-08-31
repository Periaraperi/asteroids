#pragma once

#include <functional>
#include <memory>
#include <vector>

#include "asteroid.hpp"
#include "weapons.hpp"

class Graphics;
class Input_Manager;
class Ship;
class Asteroid;
class Bullet;

class Game {
public:
    enum class Game_State {
        MAIN_MENU = 0,
        PLAYING,
        DEAD,
        WON
    };

    struct Collectible {
        glm::vec2 pos;
        glm::vec2 size;
        bool taken = false;
    };

    Game(Graphics& graphics, Input_Manager& input_manager);
    ~Game();
    
    void run();

private:
    void update(float dt);
    void render(float alpha);

    void update_main_menu_state();
    void update_playing_state(float dt);
    void update_dead_state();
    void update_won_state();

    [[nodiscard]]
    std::pair<uint32_t, uint32_t> get_world_size()
    { return {1600, 900}; }

    // TODO: in the future add more levels with different difficulty and variation.
    void init_level1();
    void init_level2();
    void init_level3();
    void init_level4();
    void init_level5();
    void test_level();
private:
    enum class Active_Weapon {
        GUN = 0,
        SHOTGUN,
        HOMING_ROCKET
    };

    bool _running;
    Game_State _state;
    Graphics& _graphics;
    Input_Manager& _input_manager;
    
    std::unique_ptr<Ship> _ship;
    std::vector<Asteroid> _asteroids;

    std::vector<Bullet> _bullets;
    std::vector<Homing_Bullet> _homing_bullets;

    Gun _gun;
    Shotgun _shotgun;

    Active_Weapon _active_weapon;

    std::vector<Collectible> _shotgun_collectibles;

    int _target_index{-1};

    std::size_t _level_id;
    std::vector<std::function<void()>> _level_init_calls;

public:
    // disable copy move ops
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;
    Game(Game&&) = delete;
    Game& operator=(Game&&) = delete;
};
