#include <SDL2/SDL.h>
#include <glm/gtc/matrix_transform.hpp>

#include <cmath>

#include "graphics.hpp"
#include "input_manager.hpp"

struct Transform {
    static glm::mat4 model(glm::vec2 scale, glm::vec2 pos, float angle = 0.0f /*in degrees*/)
    {
        // rotate around z-axis
        return glm::translate(glm::mat4(1.0f), glm::vec3(pos, 0.0f))*
               glm::rotate(glm::mat4(1.0f), Transform::to_radians(angle), glm::vec3(0.0f, 0.0f, 1.0f))*
               glm::scale(glm::mat4(1.0f), glm::vec3(scale, 1.0f));
    }

    static float to_radians(float angle)
    {
        if (angle >= FULL_ROT_ANGLE) {
            auto k {static_cast<int>(angle / FULL_ROT_ANGLE)};
            angle -= k*FULL_ROT_ANGLE;
        }
        else if (angle <= -FULL_ROT_ANGLE) {
            auto k {static_cast<int>(angle / (-FULL_ROT_ANGLE))};
            angle += k*FULL_ROT_ANGLE;
        }
        return glm::radians(angle);
    }

private:
    constexpr static float FULL_ROT_ANGLE = 360.0f;
};

int main()
{
    Graphics graphics{Window_Settings{"asteroids", 1600, 900, false, false}};
    graphics.set_clear_color(0, 0.5f, 0.5f, 1.0f);
    graphics.vsync(false);

    // order matters, First init Graphics, then rest
    Input_Manager im{};

    glm::vec2 ship_pos {800.0f, 450.0f};
    float ship_velocity = 350.0f;

    auto ship_transform = Transform::model({50.0f, 50.0f}, ship_pos);
    float ship_angle = 0.0f;

    uint32_t prev = SDL_GetTicks();

    bool running = true;
    while (running) {
        uint32_t now = SDL_GetTicks();
        float dt = (now - prev) / 1000.0f; // delta time in seconds
        prev = now;

        im.update_mouse();

        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) {
                running = false;
                break;
            } 
            else if (ev.type == SDL_WINDOWEVENT) { 
                if (ev.window.event == SDL_WINDOWEVENT_RESIZED) {
                    graphics.set_window_size(ev.window.data1, ev.window.data2);
                }
            }
        }

        if (im.key_down(SDL_SCANCODE_A)) {
            ship_angle += dt*150;
        }
        if (im.key_down(SDL_SCANCODE_D)) {
            ship_angle -= dt*150;
        }

        if (im.key_down(SDL_SCANCODE_W)) {
            // visually ship model in opengl coordinate system looks like it is already rotated by 90 degrees
            // so in order to move the position of the ship according to the tip of the triangle
            // we need to offset angles by 90 degrees
            ship_pos.x += std::cos(Transform::to_radians(ship_angle+90.0f))*ship_velocity*dt;
            ship_pos.y += std::sin(Transform::to_radians(ship_angle+90.0f))*ship_velocity*dt;
        }

        im.update_prev_state();

        ship_transform = Transform::model({50.0f, 50.0f}, ship_pos, ship_angle);

        graphics.clear_buffer();

        graphics.draw_triangle(ship_transform, {1.0f, 0.0f, 0.0f, 1.0f});

        graphics.swap_buffers();

        SDL_Delay(1);
    }

    return 0;
}
