#pragma once

#include <functional>
#include <memory>
#include <vector>

#include "asteroid.hpp"
#include "weapons.hpp"
#include "button.hpp"

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
        PAUSED,
        DEBUG_HELPER
    };

    struct Collectible {
        enum class Collectible_Type {
            SHOTGUN,
            HOMING_GUN,
        };

        Collectible() = default;
        Collectible(Collectible_Type type_, const glm::vec2& pos_, const glm::vec2& size_)
            :type{type_}, pos{pos_}, size{size_}
        {}

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
    void update_paused_state();

    void reset_state();
    void full_reset_on_dead_state();

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

    struct Upgrade {
        Upgrade(Button&& button) 
            :b{std::move(button)} 
        {}
        Button b;
        std::string on_hover_text;
        uint8_t points_needed{1};
        bool upgraded{false};
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

    // buttons
    std::vector<Upgrade> _gun_upgrades;
    std::vector<Upgrade> _shotgun_upgrades;
    std::vector<Upgrade> _homing_gun_upgrades;
    std::vector<Upgrade> _ship_speed_upgrades;
    std::vector<Upgrade> _ship_rotation_speed_upgrades;
    std::vector<Upgrade> _ship_max_health_upgrades;

    uint8_t _upgrade_count{0};
    std::array<bool, 3> _unlocked_weapons;
public:
    // disable copy move ops
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;
    Game(Game&&) = delete;
    Game& operator=(Game&&) = delete;
};
