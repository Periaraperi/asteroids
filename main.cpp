#include <SDL2/SDL.h>
#include <glm/gtc/matrix_transform.hpp>

#include "graphics.hpp"
#include "input_manager.hpp"
#include "physics.hpp"


struct Color {
    static constexpr glm::vec4 WHITE{1.0f, 1.0f, 1.0f, 1.0f};
    static constexpr glm::vec4 BLACK{0.0f, 0.0f, 0.0f, 0.0f};
    static constexpr glm::vec4 RED{1.0f, 0.0f, 0.0f, 1.0f};
    static constexpr glm::vec4 GREEN{0.0f, 1.0f, 0.0f, 1.0f};
    static constexpr glm::vec4 BLUE{0.0f, 0.0f, 1.0f, 1.0f};
    static constexpr glm::vec4 YELLOW{1.0f, 1.0f, 0.0f, 1.0f};
    static constexpr glm::vec4 MAGENTA{1.0f, 0.0f, 1.0f, 1.0f};
    static constexpr glm::vec4 CYAN{0.0f, 1.0f, 1.0f, 1.0f};
    static constexpr glm::vec4 GREY{0.5f, 0.5f, 0.5f, 1.0f};
};

struct Transform {
    // returns model matrix constructed from scale, pos, and optional angle
    static glm::mat4 model(glm::vec2 scale, glm::vec2 pos, float angle = 0.0f /*in degrees*/)
    {
        // rotate around z-axis
        return glm::translate(glm::mat4(1.0f), glm::vec3(pos, 0.0f))*
               glm::rotate(glm::mat4(1.0f), Transform::to_radians(angle), glm::vec3(0.0f, 0.0f, 1.0f))*
               glm::scale(glm::mat4(1.0f), glm::vec3(scale, 1.0f));
    }

    static float clamp_angle(float angle)
    {
        if (angle >= FULL_ROT_ANGLE) {
            auto k {static_cast<int>(angle / FULL_ROT_ANGLE)};
            angle -= k*FULL_ROT_ANGLE;
        }
        else if (angle <= -FULL_ROT_ANGLE) {
            auto k {static_cast<int>(angle / (-FULL_ROT_ANGLE))};
            angle += k*FULL_ROT_ANGLE;
        }
        return angle;
    }

    static float to_radians(float angle)
    {
        return glm::radians(clamp_angle(angle));
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

    Polygon p_triangle{{
        {400.0f, 400.0f}, {550.0f, 800.0f}, {700.0f, 400.0f}
    }};
    p_triangle.rotate(Transform::to_radians(90.0f));

    Polygon p_quad{{
        {400.0f, 400.0f},
        {400.0f, 500.0f},
        {500.0f, 500.0f},
        {500.0f, 400.0f}
    }};

    Polygon p_p1 {{
        {400.0f, 400.0f},
        {400.0f, 500.0f},
        {500.0f, 600.0f},
        {500.0f, 500.0f},
        {500.0f, 400.0f}
    }};

    Polygon static_poly {{
        {1200.0f, 500.0f},
        {1300.0f, 600.0f},
        {1400.0f, 400.0f}
    }};

    std::vector<Polygon> polys {p_triangle, p_quad, p_p1};
    std::size_t index = 0;
    
    float velocity = 250.0f;

    bool wire = false;
    bool draw_points = true;
    bool draw_normals = false;

    uint32_t prev = SDL_GetTicks();

    bool running = true;
    while (running) {
        uint32_t now = SDL_GetTicks();
        float dt = (now - prev) / 1000.0f; // delta time in seconds
        prev = now;

        bool info = false;

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

        auto [mx, my] = im.get_mouse(); 
        // convert my from SDL2 frame of reference to OpenGL
        my = graphics.get_window_size().second - my;
        graphics.set_window_title("asteroids: Mouse: x: "+std::to_string(mx)+" y: "+std::to_string(my));

        glm::vec2 mouse_vec{mx, my};

        if (im.key_pressed(SDL_SCANCODE_P)) {
            draw_points = !draw_points;
        }
        if (im.key_pressed(SDL_SCANCODE_N)) {
            draw_normals = !draw_normals;
        }
        if (im.key_pressed(SDL_SCANCODE_O)) {
            wire = !wire;
            graphics.wireframe(wire);
        }
        if (im.key_pressed(SDL_SCANCODE_I)) {
            info = true;
        }
        
        if (im.mouse_pressed(Mouse_Button::LEFT)) {
            index = (index+1)%polys.size();
        }
        auto& poly = polys[index];

        // ROTATION
        if (im.key_down(SDL_SCANCODE_Q)) {
            poly.rotate(Transform::to_radians(dt*150.0f));
        }
        if (im.key_down(SDL_SCANCODE_E)) {
            poly.rotate(Transform::to_radians(-dt*150.0f));
        }

        // MOVEMENT, doing extra work. But ok for now. We are debugging
        glm::vec2 dir{0.0f, 0.0f};
        if (im.key_down(SDL_SCANCODE_W)) {
            dir.y = 1.0f;
        }
        if (im.key_down(SDL_SCANCODE_A)) { 
            dir.x = -1.0f;
        }
        if (im.key_down(SDL_SCANCODE_S)) { 
            dir.y = -1.0f;
        }
        if (im.key_down(SDL_SCANCODE_D)) { 
            dir.x = 1.0f;
        }
        dir.x *= dt*velocity;
        dir.y *= dt*velocity;

        poly.move(dir);

        im.update_prev_state();

        graphics.clear_buffer();
        // DRAW CALLS HERE!

        if (!sat(poly.points, static_poly.points)) {
            graphics.draw_polygon(static_poly.points, Color::MAGENTA);
            graphics.draw_circle(static_poly.visual_center(), 5.0f, Color::GREEN);

            graphics.draw_polygon(poly.points, Color::RED);
            graphics.draw_circle(poly.visual_center(), 5.0f, Color::GREEN);
        }
        else {
            graphics.draw_polygon(static_poly.points, Color::WHITE);
            graphics.draw_circle(static_poly.visual_center(), 5.0f, Color::GREEN);

            graphics.draw_polygon(poly.points, Color::BLACK);
            graphics.draw_circle(poly.visual_center(), 5.0f, Color::GREEN);
        }

        if (draw_points) {
            for (const auto& p:static_poly.points) {
                graphics.draw_circle(p, 5.0f, Color::CYAN);
            }

            for (const auto& p:poly.points) {
                graphics.draw_circle(p, 5.0f, Color::BLUE);
            }
        }

        if (draw_normals) {
            for (const auto& l:normal_lines_a) {
                if (l.color.x == 0.5f)
                    graphics.draw_line(l.p1, l.p2, l.color);
                else
                    graphics.draw_line(l.p1, l.p2, Color::YELLOW);

                if (info) std::cerr << l << '\n';
            }
            
            for (const auto& l:normal_lines_b) {
                if (l.color.x == 0.5f)
                    graphics.draw_line(l.p1, l.p2, l.color);
                else 
                    graphics.draw_line(l.p1, l.p2, Color::YELLOW);

                if (info) std::cerr << l << '\n';
            }
        }

        if (info) {
            for (const auto& p:static_poly.points) {
                std::cerr << "static poly points\n";
                std::cerr << p.x << " " << p.y << '\n';
            }

            for (const auto& p:poly.points) {
                std::cerr << "player poly points\n";
                std::cerr << p.x << " " << p.y << '\n';
            }
        }


        graphics.swap_buffers();

        SDL_Delay(1);
    }

    return 0;
}
