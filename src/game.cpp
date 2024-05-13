#include "game.hpp"

#include <SDL2/SDL.h>
#include <algorithm>

#include "graphics.hpp"
#include "input_manager.hpp"
#include "peria_logger.hpp"
#include "physics.hpp"

#include "ship.hpp"
#include "asteroid.hpp"
#include "bullet.hpp"

Game::Game(Graphics& graphics, Input_Manager& input_manager)
    :_running{true}, _state{Game_State::MAIN_MENU},
     _graphics{graphics}, _input_manager{input_manager}
{
    _bullets.reserve(512); // reserve some space since we know we will shoot a lot

    PERIA_LOG("Game ctor()");
}

Game::~Game()
{
    PERIA_LOG("Game dtor()");
}

void Game::run()
{
    uint32_t prev = SDL_GetTicks();

    while (_running) {
        uint32_t now = SDL_GetTicks();
        float dt = (now - prev) / 1000.0f; // delta time in seconds
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

        update(dt);
        _input_manager.update_prev_state();

        render();

        SDL_Delay(1);
    }
}

void Game::update(float dt)
{
    switch(_state) {
        case Game_State::MAIN_MENU:
            update_main_menu_state();
            break;
        case Game_State::PLAYING:
            update_playing_state(dt);
            break;
        case Game_State::DEAD:
            update_dead_state();
            break;
        case Game_State::WON:
            update_won_state();
            break;
    }
}

void Game::render()
{
    _graphics.clear_buffer();
    // DRAW CALLS HERE!
    
    auto [w, h] = _graphics.get_window_size();
    switch(_state) {
        case Game_State::MAIN_MENU:
        {
            _graphics.draw_text("ASTEROIDS", {w*0.5f-120, h*0.5f}, {0.0f, 0.0f, 0.0f});
        } break;
        case Game_State::PLAYING:
        {
            for (const auto& a:_asteroids) {
                a.draw(_graphics);
            }

            _ship->draw(_graphics);

            for (const auto& b:_bullets) {
                b.draw(_graphics);
            }
        } break;
        case Game_State::DEAD:
        {
            _graphics.draw_text("YOU DIED", {w*0.5f-120, h*0.5f}, {0.0f, 0.0f, 0.0f});
        } break;
        case Game_State::WON:
        {
            _graphics.draw_text("YOU WON", {w*0.5f-120, h*0.5f}, {0.0f, 0.0f, 0.0f});
        } break;
    }

    _graphics.flush();
    _graphics.swap_buffers();
}

// ============================
// Game state specific updates.
// ============================

void Game::update_main_menu_state()
{
    if (_input_manager.key_pressed(SDL_SCANCODE_SPACE)) {
        init_level();
        _state = Game_State::PLAYING;
    }
    if (_input_manager.key_pressed(SDL_SCANCODE_ESCAPE)) {
        _running = false; // quit program
    }
}

void Game::update_playing_state(float dt)
{
    for (auto& a:_asteroids) {
        a.update(_graphics, dt);
    }

    _ship->update(_graphics, _input_manager, dt);

    auto ship_poly = _ship->get_points_in_world(); // concave, manually make 2 triangles
    const auto& ship_tip = ship_poly[2]; // tip of ship in world space

    // shoot bullets
    if (_input_manager.key_pressed(SDL_SCANCODE_SPACE)) {
        _bullets.emplace_back(ship_tip, _ship->get_direction_vector());
    }

    for (auto& b:_bullets) {
        b.update(_graphics, dt);
    }

    // stores asteroids from potential split
    // which later are moved into _asteroids member
    std::vector<Asteroid> new_asteroids;

    for (auto& a:_asteroids) {
        auto asteroid_points = a.get_points_in_world();
        if (sat({ship_poly[0], ship_poly[1], ship_poly[2]}, asteroid_points)) {
            _state = Game_State::DEAD;
            return;
        }
        else if (sat({ship_poly[0], ship_poly[2], ship_poly[3]}, asteroid_points)) {
            _state = Game_State::DEAD;
            return;
        }

        for (auto& b:_bullets) {
            auto bullet_points = b.get_world_points();
            if (sat(bullet_points, asteroid_points)) {
                b.explode();
                a.explode();
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
    
    _bullets.erase(std::remove_if(_bullets.begin(), _bullets.end(), 
                   [](const Bullet& b) { return b.dead(); }),
                   _bullets.end());

    _asteroids.erase(std::remove_if(_asteroids.begin(), _asteroids.end(), 
                   [](const Asteroid a) { return a.dead(); }),
                   _asteroids.end());

    for (auto& a:new_asteroids) {
        _asteroids.emplace_back(std::move(a));
    }

    if (_asteroids.empty()) {
        _state = Game_State::WON;
    }
}

void Game::update_dead_state()
{
    if (_input_manager.key_pressed(SDL_SCANCODE_SPACE)) {
        init_level();
        _state = Game_State::PLAYING;
    }
    if (_input_manager.key_pressed(SDL_SCANCODE_ESCAPE)) {
        _state = Game_State::MAIN_MENU;
    }
}

void Game::update_won_state()
{
    if (_input_manager.key_pressed(SDL_SCANCODE_SPACE)) {
        // TODO: next level 
        init_level();
        _state = Game_State::PLAYING;
    }
    if (_input_manager.key_pressed(SDL_SCANCODE_ESCAPE)) {
        _state = Game_State::MAIN_MENU;
    }
}

void Game::init_level()
{
    auto [w, h] = _graphics.get_window_size();

    _ship = std::make_unique<Ship>(glm::vec2{w*0.5f, h*0.5f});
    
    _asteroids.clear();
    _bullets.clear();

    _asteroids.emplace_back(Asteroid::Asteroid_Type::LARGE, 
                            glm::vec2{350.0f, 600.0f},
                            glm::vec2{std::cos(glm::radians(-30.0f)), std::sin(glm::radians(-30.0f))});

    _asteroids.emplace_back(Asteroid::Asteroid_Type::LARGE,
                            glm::vec2{_graphics.get_window_size().first-350.0f, 600.0f},
                            glm::vec2{std::cos(glm::radians(210.0f)), std::sin(glm::radians(210.0f))});
}
