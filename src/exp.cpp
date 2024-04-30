#include "exp.hpp"

#include <SDL2/SDL.h>

#include "graphics.hpp"
#include "input_manager.hpp"
#include "peria_logger.hpp"

#include "ship.hpp"
#include "asteroid.hpp"
#include "bullet.hpp"

Exp::Exp(Graphics& graphics, Input_Manager& input_manager)
    :_running{true}, 
     _graphics{graphics}, _input_manager{input_manager}
{

    PERIA_LOG("Exp ctor()");
}

Exp::~Exp()
{
    PERIA_LOG("Exp dtor()");
}

void Exp::run()
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

void Exp::update(float dt)
{
}

void Exp::render()
{
    _graphics.clear_buffer();
    // DRAW CALLS HERE!
    int x = 50;
    for (int i{}; i<10000; ++i) {
        _graphics.draw_triangle_batched({100.0f+i*x, 100.0f}, {100.0f+i*x, 200.0f}, {120.0f+i*x, 100.0f}, {1.0f, 0.0f, 1.0f, 1.0f});
    }

    glm::vec2 rect_size = {50, 70};
    for (int i{}; i<10000; ++i) {
        _graphics.draw_rect_batched({100.0f+i*(x+rect_size.x), 600.0f}, rect_size, {0.70f, 0.2f, 0.3f, 1.0f});
    }

    //_graphics.draw_rect_batched({100.0f, 600.0f}, rect_size, {0.70f, 0.2f, 0.3f, 1.0f});
    //_graphics.draw_rect_batched({500.0f, 600.0f}, rect_size, {0.70f, 0.2f, 0.3f, 1.0f});
    //_graphics.draw_rect_batched({100.0f, 900.0f}, rect_size, {0.70f, 0.2f, 0.3f, 1.0f});


    //for (int i{}; i<10000; ++i) {
    //    _graphics.draw_triangle_non_batched({100.0f+i*x, 100.0f}, {100.0f+i*x, 200.0f}, {120.0f+i*x, 100.0f}, {1.0f, 0.0f, 1.0f, 1.0f});
    //}

    _graphics.flush(); // this will actually make a draw call on batched data
    _graphics.swap_buffers();
}
