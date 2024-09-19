#include "game.hpp"

#include <SDL2/SDL.h>
#include <algorithm>

#include "graphics.hpp"
#include "input_manager.hpp"
#include "peria_logger.hpp"
#include "peria_utils.hpp"
#include "physics.hpp"

#include "ship.hpp"
#include "asteroid.hpp"
#include "bullet.hpp"
#include "homing_bullet.hpp"

#include "helper.hpp"

auto dt_copy = 0.0f;
Game::Game(Graphics& graphics, Input_Manager& input_manager)
    :_running{true}, _state{Game_State::MAIN_MENU},
     _graphics{graphics}, _input_manager{input_manager}, 
     _active_weapon{Active_Weapon::GUN},
     _level_id{0}
{
    _bullets.reserve(512); // reserve some space since we know we will shoot a lot
    _level_init_calls.reserve(5);
    _level_init_calls.push_back(std::bind(&Game::init_level1, this));
    _level_init_calls.push_back(std::bind(&Game::init_level2, this));
    _level_init_calls.push_back(std::bind(&Game::init_level3, this));
    _level_init_calls.push_back(std::bind(&Game::init_level4, this));
    _level_init_calls.push_back(std::bind(&Game::init_level5, this));

    PERIA_LOG("Game ctor()");
}

Game::~Game()
{
    PERIA_LOG("Game dtor()");
}

std::string info = "";

void Game::run()
{
    uint32_t prev = SDL_GetTicks();
    float accumulator = 0.0f;
    constexpr float step = 1.0f/60.0f;

    bool vsync = _graphics.get_vsync() == 0 ? false : true;

    while (_running) {
        uint32_t now = SDL_GetTicks();
        float frame_time = (now - prev) / 1000.0f; // delta time in seconds
        dt_copy = frame_time;
        prev = now;

        _input_manager.update_mouse();

        for (SDL_Event ev; SDL_PollEvent(&ev);) {
            if (ev.type == SDL_QUIT) {
                _running = false;
                break;
            } 
            else if (ev.type == SDL_WINDOWEVENT) { 
                if (ev.window.event == SDL_WINDOWEVENT_RESIZED) {
                    _graphics.set_window_size(ev.window.data1, ev.window.data2);
                }
            }
        }

        accumulator += frame_time;

        // fixed loop here
        while (accumulator >= step) {
            info = "Dt: " + std::to_string(frame_time) + " Accum: " + std::to_string(accumulator);
            if (_input_manager.key_pressed(SDL_SCANCODE_F)) {
                _graphics.toggle_fullscreen();
            }

            if (_input_manager.key_pressed(SDL_SCANCODE_V)) {
                vsync = !vsync;
                _graphics.vsync(vsync);
            }

            // do physics and game logic updates here
            update(step);
            accumulator -= step;

            _input_manager.update_prev_state();
        }

        auto alpha = accumulator / step;

        render(alpha);

        SDL_Delay(1);
    }
}

void Game::render(float alpha)
{
    glm::vec3 text_color{1.0f, 1.0f, 1.0f};
    _graphics.bind_fbo(); // draw to offscreen texture

    // DRAW CALLS HERE!

    auto [w, h] = get_world_size();
    static glm::vec2 pos{0.5f*w-300.0f, 0.5f*h};

    switch(_state) {
        case Game_State::MAIN_MENU:
            _graphics.draw_text("Asteroids", {w*0.5f - 120.0f, h*0.5f}, text_color);
            break;
        case Game_State::PLAYING:
        {
            for (const auto& a:_asteroids) {
                a.draw(_graphics, alpha);
            }

            _ship->draw(_graphics, alpha);

            for (const auto& c:_gun_collectibles) {
                if (c.type==Collectible::Collectible_Type::SHOTGUN)
                    _graphics.draw_rect(c.pos, c.size, {1.0f, 1.0f, 0.0f, 1.0f});
                else
                    _graphics.draw_rect(c.pos, c.size, {0.4f, 1.0f, 0.4f, 1.0f});
            }

            for (const auto& b:_bullets) {
                b.draw(_graphics, alpha);
            }

            for (const auto& b:_homing_bullets) {
                b.draw(_graphics, alpha);
            }

            { // draw ship hp points
                float radius = 15.0f;
                for (auto hp=_ship->hp(); hp>0; --hp) {
                    _graphics.draw_circle({w-hp*32.0f, h-30.0f}, radius, {0.863f, 0.078f, 0.235f, 1.0f});
                }
            }

            _graphics.draw_text("Asteroids Left: " + std::to_string(_asteroids.size()), {0.0f, h-30}, text_color, 0.7f);
            if (_active_weapon == Active_Weapon::SHOTGUN) {
                _graphics.draw_text("Shotgun: " + std::to_string(static_cast<int>(_shotgun.timer())), {600.0f, h-30}, text_color, 0.7f);
            }
            if (_active_weapon == Active_Weapon::HOMING_GUN) {
                _graphics.draw_text("HomingGun: " + std::to_string(static_cast<int>(_homing_gun.timer())), {600.0f, h-30}, text_color, 0.7f);
            }
        } break;
        case Game_State::DEAD:
        {
            _graphics.draw_text("YOU LOST", {w*0.5f - 120.0f, h*0.5f}, text_color);
        } break;
        case Game_State::WON:
        {
            _graphics.draw_text("YOU WON", {w*0.5f - 120.0f, h*0.5f}, text_color);
        } break;
        case Game_State::DEBUG_HELPER:
        #ifdef PERIA_DEBUG
            peria::draw(_graphics, _input_manager);
        #endif
            break;
    }

#ifdef PERIA_DEBUG
    _graphics.draw_text("Dt: "+std::to_string(dt_copy), {100,150}, text_color, 0.6f);
    if (_graphics.is_fullscreen()) {
        _graphics.draw_text("Fullscreen: True", {100,100}, text_color, 0.6f);
    }
    else {
        _graphics.draw_text("Fullscreen: False", {100,100}, text_color, 0.6f);
    }
    if (_graphics.get_vsync()==1) {
        _graphics.draw_text("Vsync: On", {100,50}, text_color, 0.6f);
    }
    else if (_graphics.get_vsync()==0) {
        _graphics.draw_text("Vsync: Off", {100,50}, text_color, 0.6f);
    }
    else if (_graphics.get_vsync()==-1) {
        _graphics.draw_text("Vsync: 3rd option", {100,50}, text_color, 0.6f);
    }
    _graphics.draw_text(info, {100,20}, text_color, 0.4f);
#endif

    _graphics.flush(); // actually draws stuff to separate fbo color attachment

    _graphics.render_to_screen();

    _graphics.swap_buffers();
}

void Game::update(float dt)
{
    switch(_state) {
        case Game_State::MAIN_MENU:
        {
            update_main_menu_state();
        } break;
        case Game_State::PLAYING:
            update_playing_state(dt);
            break;
        case Game_State::DEAD:
            update_dead_state();
            break;
        case Game_State::WON:
            update_won_state();
            break;
        case Game_State::DEBUG_HELPER:
        #ifdef PERIA_DEBUG
            if (_input_manager.key_pressed(SDL_SCANCODE_ESCAPE)) {
                _state = Game_State::MAIN_MENU;
                break;
            }
            peria::update(dt, _graphics, _input_manager);
        #endif
            break;
    }
}

// ============================
// Game state specific updates.
// ============================

void Game::update_main_menu_state()
{
    if (_input_manager.key_pressed(SDL_SCANCODE_RETURN) || _input_manager.key_pressed(SDL_SCANCODE_RETURN2)) {
        _level_init_calls[_level_id]();
        _state = Game_State::PLAYING;
    }
    if (_input_manager.key_pressed(SDL_SCANCODE_ESCAPE)) {
        _running = false; // quit program
    }
#ifdef PERIA_DEBUG
    if (_input_manager.key_pressed(SDL_SCANCODE_M)) {
        _state = Game_State::DEBUG_HELPER;
    }
#endif
}

void Game::update_playing_state(float dt)
{
    // do weapon update based on currently active weapon
    switch (_active_weapon) {
        case Active_Weapon::GUN:
            _gun.update(dt);
            break;
        case Active_Weapon::SHOTGUN:
            _shotgun.update(dt);
            if (_shotgun.timer() <= 0.0f) {
                _active_weapon = Active_Weapon::GUN;
                _gun.reset();
            }
            break;
        case Active_Weapon::HOMING_GUN:
            _homing_gun.update(dt);
            if (_homing_gun.timer() <= 0.0f) {
                _active_weapon = Active_Weapon::GUN;
                _gun.reset();
            }
            break;
        default:
            PERIA_LOG("WTF?");
    }

    for (auto& a:_asteroids) { 
        a.update(_graphics, dt);
    }

    _ship->update(_graphics, _input_manager, dt);

    // this is polygon collider for the ship
    // Note that since we don't use sprites, entities visual and colliders are the same
    peria::Polygon ship_poly{_ship->get_points_in_world()};
    const auto& ship_tip = ship_poly.points()[2]; // tip of ship in world space
    
    for (auto& c:_gun_collectibles) {
        peria::Polygon collectibe_poly{{
            {c.pos.x, c.pos.y},
            {c.pos.x+c.size.x, c.pos.y},
            {c.pos.x+c.size.x, c.pos.y-c.size.y},
            {c.pos.x, c.pos.y-c.size.y}
        }};

        // take shotgun
        if (concave_sat(ship_poly, collectibe_poly)) {
            switch (c.type) {
                case Collectible::Collectible_Type::SHOTGUN:
                    _active_weapon = Active_Weapon::SHOTGUN;
                    _shotgun.reset();
                    break;
                case Collectible::Collectible_Type::HOMING_GUN:
                    _active_weapon = Active_Weapon::HOMING_GUN;
                    _homing_gun.reset();
                    break;
                default:
                    break;
            }
            c.taken = true;
        }
    }

    // remove collected collectibles
    _gun_collectibles.erase(std::remove_if(_gun_collectibles.begin(), _gun_collectibles.end(), 
                   [](const Collectible& c) { return c.taken; }),
                   _gun_collectibles.end());

    // logic for bullets shooting based on weapon
    {
        if (_input_manager.key_down(SDL_SCANCODE_SPACE)) {
            switch (_active_weapon) {
                case Active_Weapon::GUN:
                    if (_gun.delay() <= 0.0f) {
                        _gun.shoot(ship_tip, _ship->get_direction_vector(), _bullets);
                    }
                    break;
                case Active_Weapon::SHOTGUN:
                    if (_shotgun.delay() <= 0.0f) {
                        _shotgun.shoot(ship_tip, _ship->get_direction_vector(), _bullets);
                    }
                    break;
                case Active_Weapon::HOMING_GUN:
                    {
                    _target_index = _homing_gun.search(ship_tip, _asteroids);
                    _asteroids[_target_index].set_color({1.0f, 0.5f, 1.0f, 1.0f});
                    }break;
                default:
                    PERIA_LOG("WTF?");
            }
        }

        if (_input_manager.key_released(SDL_SCANCODE_SPACE) &&
            _active_weapon == Active_Weapon::HOMING_GUN) {
            _homing_bullets.emplace_back(ship_tip, 7.0f, _target_index, _ship->get_direction_vector(), _ship->get_angle(), glm::vec4{1.0f, 1.0f, 0.0f, 1.0f});
            _target_index = -1;
        }
    }

    for (auto& b:_bullets) {
        b.update(dt);
    }

    for (auto& hb:_homing_bullets) {
        const auto target_index = hb.get_target_index();
        if (target_index>=0 && target_index<static_cast<int>(_asteroids.size())) {
            hb.set_target_pos(_asteroids[target_index].get_world_pos());
            _asteroids[target_index].set_color({1.0f, 0.5f, 1.0f, 1.0f});
        }
        hb.update(dt);
    }

    // stores asteroids from potential split
    // which later are moved into _asteroids member
    std::vector<Asteroid> new_asteroids;
    new_asteroids.reserve(32);

    // helper lambda for collectibles
    auto spawn_collectible = [this](const Asteroid& a) {
        if (peria::get_int(1, 8) == 8) {
            _gun_collectibles.push_back({Collectible::Collectible_Type::SHOTGUN, a.get_world_pos(), {10.0f, 10.0f}});
        }
        else if (peria::get_int(1, 8) == 8) {
            _gun_collectibles.push_back({Collectible::Collectible_Type::HOMING_GUN, a.get_world_pos(), {10.0f, 10.0f}});
        }
    };

    // check collisions between asteroids and other entities
    {
        for (auto& a:_asteroids) {
            const peria::Polygon asteroid_poly{a.get_points_in_world()};

            if (!_ship->is_invincible()) {
                if (concave_sat(ship_poly, asteroid_poly)) {
                    _ship->hit();
                    if (_ship->hp() == 0) {
                        _state = Game_State::DEAD;
                        return;
                    }
                }
            }

            for (auto& b:_bullets) {
                if (a.dead()) continue;
                
                peria::Polygon bullet_poly{b.get_world_points()};
                if (concave_sat(bullet_poly, asteroid_poly)) {
                    b.explode();
                    a.hit(); // deal damage
                    if (a.hp() == 0) {
                        a.explode();
                        // randomly drop collectibles after asteroid explodes
                        spawn_collectible(a);
                        auto asteroids = a.split(); // vector of 0, 3 or 6 asteroids
                        // move temporary smaller asteroids into new_asteroids
                        if (!asteroids.empty()) {
                            for (auto& tmp:asteroids) {
                                new_asteroids.emplace_back(std::move(tmp));
                            }
                        }
                    }
                }
            }

            for (auto& hb:_homing_bullets) {
                if (a.dead()) continue;
                
                peria::Polygon bullet_poly{hb.get_world_points()};
                if (concave_sat(bullet_poly, asteroid_poly)) {
                    hb.explode();
                    a.hit(); // deal damage
                    if (a.hp() == 0) {
                        a.explode();
                        // randomly drop collectibles after asteroid explodes
                        spawn_collectible(a);
                        auto asteroids = a.split(); // vector of 0 or 3 or 6 asteroids
                        // move temporary smaller asteroids into new_asteroids
                        if (!asteroids.empty()) {
                            for (auto& tmp:asteroids) {
                                new_asteroids.emplace_back(std::move(tmp));
                            }
                        }
                    }
                }
            }
        }
    }
    
    _bullets.erase(std::remove_if(_bullets.begin(), _bullets.end(), 
                   [](const Bullet& b) { return b.dead(); }),
                   _bullets.end());

    // before erasing dead asteroids check 2 scenarios:
    // 1) homing bullets target asteroid was exploded by other bullet
    //    in previous frame. In this case unset hb target and let it continue flying
    //    in last direction it was flying.
    // 2) homing bullet is targeting asteroid x, in prev frame another bullet killed
    //    some other asteroid y. If Index of y < Index of x, after vector.erase()
    //    hb target will point to invalid asteroid, or even could be out of bounds.
    //    So offset all the prev target indices that are effected by resize.
    {
        // each i-th elem stores number of dead asteroids in that prefix 
        // NOTE: using 1-based indexing here
        std::vector<int> pref_dead_asteroids(_asteroids.size()+1, 0);

        for (std::size_t i{}; i<_asteroids.size(); ++i) {
            if (_asteroids[i].dead()) pref_dead_asteroids[i+1] = 1;
            pref_dead_asteroids[i+1] += pref_dead_asteroids[i];
        }

        for (std::size_t i{}; i<_homing_bullets.size(); ++i) {
            auto& hb = _homing_bullets[i];
            if (const auto target_index = hb.get_target_index(); 
                target_index>=0 && target_index<static_cast<int>(_asteroids.size())) {
                if (_asteroids[target_index].dead() && !hb.dead()) { // case 1
                    hb.set_target_index(-1);
                }
                else { // case 2
                    hb.set_target_index(target_index - pref_dead_asteroids[target_index+1]); // +1 because pref indexing is 1-based
                }
            }
        }
    }

    _asteroids.erase(std::remove_if(_asteroids.begin(), _asteroids.end(), 
                   [](const Asteroid& a) { return a.dead(); }),
                   _asteroids.end());

    _homing_bullets.erase(std::remove_if(_homing_bullets.begin(), _homing_bullets.end(), 
                   [](const Homing_Bullet& hb) { return hb.dead(); }),
                   _homing_bullets.end());

    for (auto& a:new_asteroids) {
        _asteroids.emplace_back(std::move(a));
    }

    if (_asteroids.empty()) {
        _state = Game_State::WON;
    }
}

void Game::update_dead_state()
{
    _level_id = 0;
    if (_input_manager.key_pressed(SDL_SCANCODE_RETURN) || _input_manager.key_pressed(SDL_SCANCODE_RETURN2)) {
        _level_init_calls[_level_id]();
        _state = Game_State::PLAYING;
    }
    if (_input_manager.key_pressed(SDL_SCANCODE_ESCAPE)) {
        _state = Game_State::MAIN_MENU;
    }
}

void Game::update_won_state()
{
    if (_input_manager.key_pressed(SDL_SCANCODE_RETURN) || _input_manager.key_pressed(SDL_SCANCODE_RETURN2)) {
        if (_level_id < _level_init_calls.size()-1) {
            ++_level_id;
        }
        _level_init_calls[_level_id]();
        _state = Game_State::PLAYING;
    }
    if (_input_manager.key_pressed(SDL_SCANCODE_ESCAPE)) {
        _state = Game_State::MAIN_MENU;
    }
}

void Game::reset_state()
{
    auto [w, h] = get_world_size();
    _ship = std::make_unique<Ship>(glm::vec2{w*0.5f, h*0.5f});
    
    _asteroids.clear();
    _bullets.clear();
    _homing_bullets.clear();
    _gun_collectibles.clear();
    _active_weapon = Active_Weapon::GUN;
    _gun.reset();
}

void Game::init_level1()
{
    reset_state();
    _asteroids.emplace_back(Asteroid::Asteroid_Type::LARGE, 
                            glm::vec2{350.0f, 600.0f},
                            glm::vec2{std::cos(glm::radians(-30.0f)), std::sin(glm::radians(-30.0f))}, 1);

    _asteroids.emplace_back(Asteroid::Asteroid_Type::LARGE,
                            glm::vec2{get_world_size().first-350.0f, 600.0f},
                            glm::vec2{std::cos(glm::radians(210.0f)), std::sin(glm::radians(210.0f))}, 1);
}

void Game::init_level2()
{
    reset_state();
    _asteroids.emplace_back(Asteroid::Asteroid_Type::LARGE, 
                            glm::vec2{350.0f, 600.0f},
                            glm::vec2{std::cos(glm::radians(-30.0f)), std::sin(glm::radians(-30.0f))}, 2);

    _asteroids.emplace_back(Asteroid::Asteroid_Type::LARGE,
                            glm::vec2{get_world_size().first-350.0f, 600.0f},
                            glm::vec2{std::cos(glm::radians(210.0f)), std::sin(glm::radians(210.0f))}, 2);
}

void Game::init_level3()
{
    reset_state();
    _asteroids.emplace_back(Asteroid::Asteroid_Type::LARGE, 
                            glm::vec2{350.0f, 600.0f},
                            glm::vec2{std::cos(glm::radians(-30.0f)), std::sin(glm::radians(-30.0f))}, 3);

    _asteroids.emplace_back(Asteroid::Asteroid_Type::LARGE,
                            glm::vec2{get_world_size().first-350.0f, 600.0f},
                            glm::vec2{std::cos(glm::radians(210.0f)), std::sin(glm::radians(210.0f))}, 3);
}

void Game::init_level4()
{
    reset_state();
    auto [w, h] = get_world_size();
    _asteroids.emplace_back(Asteroid::Asteroid_Type::LARGE, 
                            glm::vec2{350.0f, 600.0f},
                            glm::vec2{std::cos(glm::radians(-30.0f)), std::sin(glm::radians(-30.0f))}, 4);

    _asteroids.emplace_back(Asteroid::Asteroid_Type::LARGE,
                            glm::vec2{get_world_size().first-350.0f, 600.0f},
                            glm::vec2{std::cos(glm::radians(210.0f)), std::sin(glm::radians(210.0f))}, 4);

    _asteroids.emplace_back(Asteroid::Asteroid_Type::LARGE,
                            glm::vec2{w*0.5f, h*0.5f-300.0f},
                            _ship->get_direction_vector(), 4);
}

void Game::init_level5()
{
    reset_state();
    auto [w, h] = get_world_size();
    _asteroids.emplace_back(Asteroid::Asteroid_Type::LARGE, 
                            glm::vec2{350.0f, 600.0f},
                            glm::vec2{std::cos(glm::radians(-30.0f)), std::sin(glm::radians(-30.0f))}, 5);

    _asteroids.emplace_back(Asteroid::Asteroid_Type::LARGE,
                            glm::vec2{get_world_size().first-350.0f, 600.0f},
                            glm::vec2{std::cos(glm::radians(210.0f)), std::sin(glm::radians(210.0f))}, 5);

    _asteroids.emplace_back(Asteroid::Asteroid_Type::LARGE,
                            glm::vec2{w*0.5f, h*0.5f-300.0f},
                            _ship->get_direction_vector(), 5);
}
