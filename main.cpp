#include <SDL2/SDL.h>
#include <glm/gtc/matrix_transform.hpp>

#include "game.hpp"
#include "graphics.hpp"
#include "input_manager.hpp"

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

    Input_Manager im{};

    Game asteroids{graphics, im};
    asteroids.run();

    return 0;
}
