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
    :_graphics{graphics}, _input_manager{input_manager}
{
    auto [w, h] = _graphics.get_window_size();
    _ship = std::make_unique<Ship>(glm::vec2{w*0.5f, h*0.5f});
    
    _asteroids.reserve(2);
    _asteroids.emplace_back(std::make_unique<Asteroid>(glm::vec2{350.0f, 600.0f}));
    _asteroids.back()->set_velocity({50.0f, -35.0f});
    _asteroids.emplace_back(std::make_unique<Asteroid>(glm::vec2{_graphics.get_window_size().first-350.0f, 600.0f}));
    _asteroids.back()->set_velocity({-160.0f, -25.0f});

    _bullets.reserve(300); // reserve some space since we know we will shoot a lot

    PERIA_LOG("Game ctor()");
}

Game::~Game()
{
    PERIA_LOG("Game dtor()");
}

void Game::run()
{
    uint32_t prev = SDL_GetTicks();

    for (bool running=true; running;) {
        uint32_t now = SDL_GetTicks();
        float dt = (now - prev) / 1000.0f; // delta time in seconds
        prev = now;

        _input_manager.update_mouse();

        for (SDL_Event ev; SDL_PollEvent(&ev);) {
            if (ev.type == SDL_QUIT) {
                running = false;
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
    for (const auto& a:_asteroids) {
        a->update(_graphics, dt);
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

    for (const auto& a:_asteroids) {
        auto asteroid_points = a->get_points_in_world();
        if (sat({ship_poly[0], ship_poly[1], ship_poly[2]}, asteroid_points)) {
            PERIA_LOG("SHIP DESTROYED!!!");
        }
        else if (sat({ship_poly[0], ship_poly[2], ship_poly[3]}, asteroid_points)) {
            PERIA_LOG("SHIP DESTROYED!!!");
        }

        for (auto& b:_bullets) {
            auto bullet_points = b.get_world_points();
            if (sat(bullet_points, asteroid_points)) {
                b.explode();
                a->explode();
                auto [a1, a2] = a->split();
                auto pos = a->get_world_pos(); // gives center
                if (!a1.empty() && !a2.empty()) {
                    _asteroids.emplace_back(std::make_unique<Asteroid>(a1, pos+glm::vec2{0.0f, 50.0f}));
                    _asteroids.emplace_back(std::make_unique<Asteroid>(a2, pos+glm::vec2{0.0f, -50.0f}));
                }
            }
        }
    }
    
    _bullets.erase(std::remove_if(_bullets.begin(), _bullets.end(), 
                   [](const Bullet& b) { return b.dead(); }),
                   _bullets.end());

    _asteroids.erase(std::remove_if(_asteroids.begin(), _asteroids.end(), 
                   [](const std::unique_ptr<Asteroid>& a) { return a->dead(); }),
                   _asteroids.end());
}

void Game::render()
{
    _graphics.clear_buffer();
    // DRAW CALLS HERE!
    
    for (const auto& a:_asteroids) {
        a->draw(_graphics);
    }

    _ship->draw(_graphics);

    for (const auto& b:_bullets) {
        b.draw(_graphics);
    }

    _graphics.swap_buffers();
}
