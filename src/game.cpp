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
    auto [w, h] = _graphics.get_window_size();
    _ship = std::make_unique<Ship>(glm::vec2{w*0.5f, h*0.5f});
    
    _asteroids.reserve(2);
    _asteroids.emplace_back(std::make_unique<Asteroid>(glm::vec2{350.0f, 600.0f}));
    _asteroids.back()->set_velocity({50.0f, -35.0f});
    _asteroids.emplace_back(std::make_unique<Asteroid>(glm::vec2{_graphics.get_window_size().first-350.0f, 600.0f}));
    _asteroids.back()->set_velocity({-160.0f, -25.0f});

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

    //if (_input_manager.key_pressed(SDL_SCANCODE_B)) {
    //    if (!_asteroids.empty()) {
    //        auto [a1, a2] = _asteroids[0]->split();
    //        auto pos = _asteroids[0]->get_world_pos(); // gives center
    //        _asteroids.erase(_asteroids.begin());
    //        if (!a1.empty() && !a2.empty()) {
    //            _asteroids.emplace_back(std::make_unique<Asteroid>(a1, pos+glm::vec2{0.0f, 50.0f}));
    //            _asteroids.emplace_back(std::make_unique<Asteroid>(a2, pos+glm::vec2{0.0f, -50.0f}));
    //        }
    //        else {
    //            PERIA_LOG("GATAVDA!!!");
    //        }
    //    }
    //}
    _ship->update(_graphics, _input_manager, dt);

    // check collision here for now
    for (const auto& a:_asteroids) {
        auto ship_poly = _ship->get_points_in_world(); // concave, manually make 2 triangles
        if (sat({ship_poly[0], ship_poly[1], ship_poly[2]}, a->get_points_in_world())) {
            PERIA_LOG("COLLISION 1!!!");
        }
        else if (sat({ship_poly[0], ship_poly[2], ship_poly[3]}, a->get_points_in_world())) {
            PERIA_LOG("COLLISION 2!!!");
        }
    }

}

void Game::render()
{
    _graphics.clear_buffer();
    // DRAW CALLS HERE!
    
    for (const auto& a:_asteroids) {
        a->draw(_graphics);
    }

    _ship->draw(_graphics);

    _graphics.swap_buffers();
}
