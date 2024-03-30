#include <SDL2/SDL.h>
#include <glm/gtc/matrix_transform.hpp>

#include <cmath>
#include <iostream>
#include <array>

#include "graphics.hpp"
#include "input_manager.hpp"
#include "physics.hpp"

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

constexpr std::array<glm::vec4, 4> circle_colors {{
    {0.0f, 0.0f, 1.0f, 1.0f},
    {1.0f, 1.0f, 0.0f, 1.0f},
    {1.0f, 0.0f, 1.0f, 1.0f},
    {0.0f, 1.0f, 1.0f, 1.0f},
}};

constexpr std::array<const char*, 4> color_names {
    "blue",
    "yellow",
    "purple",
    "cyan"
};

int main()
{
    Graphics graphics{Window_Settings{"asteroids", 1600, 900, false, false}};
    graphics.set_clear_color(0, 0.5f, 0.5f, 1.0f);
    graphics.vsync(true);

    // order matters, First init Graphics, then rest
    Input_Manager im{};

    glm::vec2 ship_pos {800.0f, 450.0f};
    float ship_velocity = 350.0f;

    auto ship_transform = Transform::model({50.0f, 50.0f}, ship_pos);
    float ship_angle = 0.0f;

    // poly
    std::vector<glm::vec2> points {
        {1000.0f, 450.0f}, 
        {1200.0f, 650.0f},
        {1400.0f, 120.0f},
        {1150.0f, 50.0f},
        {800.0f, 100.0f}
    };

    // convex polys
    // poly1 is fixed in the world
    std::vector<glm::vec2> poly1 {
        {100.0f, 100.0f},
        {150.0f, 200.0f},
        {300.0f, 170.0f},
        {500.0f, 50.0f}
    };
    for (auto& p:poly1) p += glm::vec2{400.0f, 400.0f};
    // poly2 will follow mouse.
    // add mouse offset to below points
    std::vector<glm::vec2> poly2_model {
        {0.0f, 0.0f}, // mouse must be here
        {150.0f, 160.0f},
        {300.0f, -100.0f},
        {140.0f, -140.0f}
    };

    // add other polys for testing
    std::vector<glm::vec2> rect1 {
        {300.0f, 700.0f},
        {500.0f, 700.0f},
        {500.0f, 400.0f},
        {300.0f, 400.0f},
    };
    glm::vec2 cc = {400.0f, 550.0f};

    for (auto& p:rect1) {
        p -= cc;
    }
    
    // rotate by 45 degrees clockwise around center of rect
    auto aa = Transform::to_radians(45.0f);
    for (auto& p:rect1) {
        p.x = std::cos(aa)*p.x - std::sin(aa)*p.y;
        p.y = std::sin(aa)*p.x + std::cos(aa)*p.y;
    }

    for (auto& p:rect1) {
        p += cc;
    }

    std::vector<glm::vec2> rect2_model {
        {0.0f, 0.0f},
        {100.0f, 0.0f},
        {100.0f, 100.0f},
        {0.0f, 100.0f},
    };

    std::vector<glm::vec2> quad {{100.0f, 100.0f}, {100.0f, 150.0f}, {150.0f, 150.0f}, {150.0f, 100.0f}};

    bool toggle = true;
    bool wire = false;
    bool draw_points = true;
    bool draw_normals = false;

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

        auto [mx, my] = im.get_mouse(); 
        // convert my from SDL2 frame of reference to OpenGL
        my = graphics.get_window_size().second - my;
        graphics.set_window_title("asteroids: Mouse: x: "+std::to_string(mx)+" y: "+std::to_string(my));

        if (im.key_pressed(SDL_SCANCODE_Y)) {
            draw_normals = !draw_normals;
        }
        if (im.key_pressed(SDL_SCANCODE_U)) {
            draw_points = !draw_points;
        }
        if (im.key_pressed(SDL_SCANCODE_P)) {
            toggle = !toggle;
        }
        if (im.key_pressed(SDL_SCANCODE_O)) {
            wire = !wire;
            graphics.wireframe(wire);
        }

        if (toggle) {
            if (im.key_down(SDL_SCANCODE_A)) {
                ship_angle += dt*150; Transform::clamp_angle(ship_angle);
            }
            if (im.key_down(SDL_SCANCODE_D)) {
                ship_angle -= dt*150; Transform::clamp_angle(ship_angle);
            }

            if (im.key_down(SDL_SCANCODE_W)) {
                // visually ship model in opengl coordinate system looks like it is already rotated by 90 degrees
                // so in order to move the position of the ship according to the tip of the triangle
                // we need to offset angles by 90 degrees
                ship_pos.x += std::cos(Transform::to_radians(ship_angle+90.0f))*ship_velocity*dt;
                ship_pos.y += std::sin(Transform::to_radians(ship_angle+90.0f))*ship_velocity*dt;
            }
        }
        
        glm::vec2 mouse_vec{mx, my};

        std::vector<glm::vec2> poly2(4);
        for (std::size_t i{}; i<poly2.size(); ++i) {
            poly2[i] = poly2_model[i] + mouse_vec;
        }

        std::vector<glm::vec2> rect2(4);
        for (std::size_t i{}; i<rect2.size(); ++i) {
            rect2[i] = rect2_model[i] + mouse_vec;
        }

        // info on last sat test
        if (im.key_pressed(SDL_SCANCODE_V)) {
            std::cerr << "================================================\n";
            for (std::size_t i{}; i<normal_lines.size(); ++i) {
                std::cerr << "-------------------------------------------------------------------\n";
                std::cerr << "LINE COLOR: " << color_names[i%color_names.size()] << '\n';
                std::cerr << normal_lines[i].p1.x << " " << normal_lines[i].p1.y << '\n';
                std::cerr << normal_lines[i].p2.x << " " << normal_lines[i].p2.y << '\n';
                std::cerr << "PROJECTED INTERVALS\n";
                std::cerr << normal_lines[i].mina << " " << normal_lines[i].maxa << '\n';
                std::cerr << normal_lines[i].minb << " " << normal_lines[i].maxb << '\n';
                std::cerr << "is maxa < minb? ";
                if (normal_lines[i].maxa < normal_lines[i].minb) std::cerr << "YES\n";
                else                                             std::cerr << "NO\n";
                std::cerr << "is maxb < mina? ";
                if (normal_lines[i].maxb < normal_lines[i].mina) std::cerr << "YES\n";
                else                                             std::cerr << "NO\n";
                std::cerr << "-------------------------------------------------------------------\n";
            }
            std::cerr << "================================================\n";
        }

        im.update_prev_state();

        if (toggle)
            ship_transform = Transform::model({50.0f, 50.0f}, ship_pos, ship_angle);

        graphics.clear_buffer();

        if (toggle)
            graphics.draw_triangle(ship_transform, {1.0f, 0.0f, 0.0f, 1.0f});
        else 
            graphics.draw_polygon(points, {0.1f, 1.0f, 0.2f, 1.0f});

        //graphics.draw_circle({mx, my}, 50.0f, {1.0f, 0.5f, 0.5f, 1.0f});
        //graphics.draw_polygon({{100.0f, 100.0f}, {100.0f, 150.0f}, {150.0f, 150.0f}, {150.0f, 100.0f}}, {0.0f, 1.0f, 0.5f, 1.0f});
        //if (aabb(rect1, rect2)) {
        //    graphics.draw_rect(rect1.pos, rect1.size, {1.0f, 1.0f, 1.0f, 1.0f});
        //    graphics.draw_rect(rect2.pos, rect2.size, {0.0f, 0.0f, 0.0f, 1.0f});
        //}
        //else {
        //    graphics.draw_rect(rect1.pos, rect1.size, {1.0f, 1.0f, 0.0f, 1.0f});
        //    graphics.draw_rect(rect2.pos, rect2.size, {0.0f, 1.0f, 1.0f, 1.0f});
        //}

        if (!sat(poly1, poly2)) {
            graphics.draw_polygon(poly1, {0.60f, 0.20f, 0.0f, 1.0f});
            graphics.draw_polygon(poly2, {0.10f, 0.90f, 0.15f, 1.0f});
        }
        else {
            graphics.draw_polygon(poly1, {1.0f, 1.0f, 1.0f, 1.0f});
            graphics.draw_polygon(poly2, {0.0f, 0.0f, 0.0f, 1.0f});
        }
        
        //if (!sat(rect1, rect2)) {
        //    graphics.draw_polygon(rect1, {0.60f, 0.20f, 0.0f, 1.0f});
        //    graphics.draw_polygon(rect2, {0.10f, 0.90f, 0.15f, 1.0f});
        //}
        //else {
        //    graphics.draw_polygon(rect1, {1.0f, 1.0f, 1.0f, 1.0f});
        //    graphics.draw_polygon(rect2, {0.0f, 0.0f, 0.0f, 1.0f});
        //}

        // debug SAT
        if (draw_points) {
            for (std::size_t i{}; i<poly1.size(); ++i) {
                graphics.draw_circle(poly1[i], 7.0f, circle_colors[i]);
            }
            for (std::size_t i{}; i<poly2.size(); ++i) {
                graphics.draw_circle(poly2[i], 7.0f, circle_colors[i]);
            }
        }
        //if (draw_points) {
        //    for (std::size_t i{}; i<rect1.size(); ++i) {
        //        graphics.draw_circle(rect1[i], 7.0f, circle_colors[i]);
        //    }
        //    for (std::size_t i{}; i<rect2.size(); ++i) {
        //        graphics.draw_circle(rect2[i], 7.0f, circle_colors[i]);
        //    }
        //}

        if (draw_normals) {
            for (std::size_t i{}; i<normal_lines.size(); ++i) {
                graphics.draw_line(normal_lines[i].p1, normal_lines[i].p2, circle_colors[i%circle_colors.size()]);
            }
        }

        graphics.swap_buffers();

        SDL_Delay(1);
    }

    return 0;
}
