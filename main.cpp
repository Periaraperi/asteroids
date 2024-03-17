#include <SDL2/SDL.h>
#include <iostream>

#include "graphics.hpp"
#include "input_manager.hpp"

int main()
{
    Graphics& graphics = Graphics::Instance();
    graphics.set_clear_color(0, 0.5f, 0.5f, 1.0f);
    graphics.set_window_size(1600, 900);

    bool resizable = true;
    graphics.set_window_resizable(resizable);

    // order matters, First init Graphics, then rest
    Input_Manager& im = Input_Manager::Instance();

    bool running = true;
    while (running) {

        im.update_mouse();

        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type==SDL_QUIT) {
                running = false;
                break;
            } 
            else if (ev.type == SDL_WINDOWEVENT) { 
                if (ev.window.event == SDL_WINDOWEVENT_RESIZED) {
                    std::cerr << "Window Resize Event\n";
                    std::cerr << "New size: " << ev.window.data1 << " " << ev.window.data2 << '\n';
                    graphics.set_window_size(ev.window.data1, ev.window.data2);
                }
            }
        }
        
        im.update_prev_state();

        graphics.clear_buffer();

        graphics.swap_buffers();

        SDL_Delay(1);
    }

    return 0;
}
