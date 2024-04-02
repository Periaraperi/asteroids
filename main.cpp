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
