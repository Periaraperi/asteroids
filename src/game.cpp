#include "game.hpp"

#include <SDL2/SDL.h>

#include "graphics.hpp"
#include "input_manager.hpp"
#include "peria_logger.hpp"

#include "ship.hpp"

Game::Game(Graphics& graphics, Input_Manager& input_manager)
    :_graphics{graphics}, _input_manager{input_manager}
{
    _ship = std::make_unique<Ship>(_graphics, _input_manager);

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
    _ship->update(dt);
}

void Game::render()
{
    _graphics.clear_buffer();
    // DRAW CALLS HERE!

    _ship->draw();

    _graphics.swap_buffers();
}
