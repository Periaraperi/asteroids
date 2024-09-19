#include "helper.hpp"
#include "graphics.hpp"
#include "input_manager.hpp"

#include <vector>
#include <string>
#include <array>
#include <fstream>
#include <glm/vec2.hpp>

namespace {
    void save_points(const std::string& file_name, const std::vector<glm::vec2>& points, int w, int h)
    {
        auto normalize = [&](glm::vec2 p) {
            return glm::vec2{(p.x - 0.5f*w) / w, (p.y - 0.5f*h) / h};
        };

        // normalized points in clockwise direction
        std::fstream fs{file_name, std::ios_base::app};
        if (fs.is_open()) {
            for (const auto& p:points) {
                auto normalized = normalize(p);
                fs << '{' <<normalized.x << "f, " << normalized.y << "f},\n";
            }
            fs << "============================\n";
        }
    }
}

namespace peria {

    constexpr auto w = 1600;
    constexpr auto h = 900;
    std::vector<glm::vec2> poly_points;

    enum class Button_Type {
      SAVE = 0,
      CLEAR,
    };

    struct Button {
      glm::vec2 pos{};
      glm::vec2 dimensions{100.0f, 30.0f};
      glm::vec4 color{1.0f, 1.0f, 1.0f, 1.0};
      glm::vec4 hovered_color{0.5f, 0.5f, 0.25f, 1.0};
      std::string text;

      [[nodiscard]] bool is_hovered(int mx, int my)
      {
        return mx >= pos.x && mx <= pos.x + dimensions.x &&
               my <= pos.y && my >= pos.y - dimensions.y;
      }
    };
    std::array<Button, 2> buttons {{ {.text = "SAVE"}, {.text = "CLEAR"} }};

    glm::vec2 get_mapped_mouse(Graphics& g, Input_Manager& im)
    {
        const auto [window_w, window_h] = g.get_window_size();
        auto [mx, my] = im.get_mouse();
        return {(static_cast<float>(mx)/window_w)*w, (static_cast<float>(my)/window_h)*h};
    }

    void update(float dt, Graphics& g, Input_Manager& im)
    {
        const auto mouse = get_mapped_mouse(g, im);
        auto mx = mouse.x;
        auto my = mouse.y;
        std::cerr << mx << " " << my << '\n';
        my = h-my;

        if (buttons[0].is_hovered(mx, my) && im.mouse_pressed(Mouse_Button::LEFT)) {
            save_points("poly_points", poly_points, w, h);
        }
        if (buttons[1].is_hovered(mx, my) && im.mouse_pressed(Mouse_Button::LEFT)) {
            poly_points.clear();
        }

        if (!buttons[0].is_hovered(mx, my) && !buttons[1].is_hovered(mx, my) &&
            im.mouse_pressed(Mouse_Button::LEFT)) {
            poly_points.push_back({mx, my});
        }

        if (im.mouse_pressed(Mouse_Button::RIGHT) && !poly_points.empty()) {
            poly_points.pop_back();
        }
    }

    void draw(Graphics& g, Input_Manager& im)
    {
        const auto mouse = get_mapped_mouse(g, im);
        const auto& mx = mouse.x;
        const auto& my = mouse.y;

        g.draw_line({0.0f, h*0.5f}, {w, h*0.5f}, {1.0f, 1.0f, 1.0f, 1.0f});
        g.draw_line({w*0.5f, h}, {w*0.5f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f});

        g.draw_circle({mx, h-my}, 3.0f, {1.0f, 1.0f, 1.0f, 1.0f});

        const auto margin_y = 5.0f;
        buttons[0].pos = {0.0f, h};
        for (std::size_t i{}; i<buttons.size(); ++i) {
            auto& b = buttons[i];
            b.pos.y = h - (b.dimensions.y + margin_y)*i;

            if (b.is_hovered(mx, h-my)) g.draw_rect(b.pos, b.dimensions, b.hovered_color);
            else                        g.draw_rect(b.pos, b.dimensions, b.color);

            g.draw_text(b.text, {b.pos.x+20.0f, b.pos.y-25.0f}, {}, 0.5f);
        }

        for (const auto& p:poly_points) {
            g.draw_circle(p, 5.0f, {1.0f, 0.5f, 0.5f, 1.0f});
        }
    }
}
