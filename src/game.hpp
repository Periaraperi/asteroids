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
class Homing_Bullet;

class Game {
public:
    enum class Game_State {
        MAIN_MENU = 0,
        PLAYING,
        DEAD,
        WON,
        DEBUG_HELPER
    };

    struct Collectible {
        enum class Collectible_Type {
            SHOTGUN,
            HOMING_GUN,
        };
        Collectible_Type type;
        glm::vec2 pos;
        glm::vec2 size;
        bool taken{false};
    };

    struct World_Size {
        float x;
        float y;
    };

    Game(Graphics& graphics, Input_Manager& input_manager);
    ~Game();
    
    void run();

    [[nodiscard]]
    static World_Size get_world_size()
    { return {1600.0f, 900.0f}; }

private:
    void update(float dt);
    void render(float alpha);

    void update_main_menu_state();
    void update_playing_state(float dt);
    void update_dead_state();
    void update_won_state();


    void reset_state();
    void init_level1();
    void init_level2();
    void init_level3();
    void init_level4();
    void init_level5();
private:
    enum class Active_Weapon {
        GUN = 0,
        SHOTGUN,
        HOMING_GUN
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
    Homing_Gun _homing_gun;

    Active_Weapon _active_weapon;

    std::vector<Collectible> _gun_collectibles;

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
