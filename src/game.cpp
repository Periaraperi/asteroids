#include "game.hpp"

#include <SDL2/SDL.h>

#include "graphics.hpp"
#include "input_manager.hpp"
#include "peria_logger.hpp"
#include "physics.hpp"

#include "ship.hpp"
#include "asteroid.hpp"

Game::Game(Graphics& graphics, Input_Manager& input_manager)
    :_graphics{graphics}, _input_manager{input_manager}
{
    _ship = std::make_unique<Ship>(_graphics, _input_manager);
    
    _asteroids.reserve(2);
    _asteroids.emplace_back(std::make_unique<Asteroid>(_graphics, glm::vec2{350.0f, 600.0f}));
    _asteroids.back()->set_velocity({30.0f, -25.0f});
    _asteroids.emplace_back(std::make_unique<Asteroid>(_graphics, glm::vec2{_graphics.get_window_size().first-350.0f, 600.0f}));
    _asteroids.back()->set_velocity({-30.0f, -25.0f});

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
        a->update(dt);
    }
    _ship->update(dt);

    // check collision here for now
    for (const auto& a:_asteroids) {
        auto a_poly = a->get_points_in_world();
        auto ship_poly = _ship->get_points_in_world(); // concave, manually make 2 triangles
        if (sat({ship_poly[0], ship_poly[1], ship_poly[2]}, a_poly)) {
            PERIA_LOG("COLLISION 1!!!");
        }
        else if (sat({ship_poly[0], ship_poly[2], ship_poly[3]}, a_poly)) {
            PERIA_LOG("COLLISION 2!!!");
        }
    }
}

void Game::render()
{
    _graphics.clear_buffer();
    // DRAW CALLS HERE!
    
    for (const auto& a:_asteroids) {
        a->draw();
    }

    _ship->draw();

    _graphics.swap_buffers();
}
