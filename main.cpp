#include <SDL2/SDL.h>
#include <glm/gtc/matrix_transform.hpp>

#include "game.hpp"
#include "graphics.hpp"
#include "input_manager.hpp"

int main(int argc, char** argv)
{
    Graphics graphics{Window_Settings{"asteroids", 1600, 900, false, true}};
    graphics.set_clear_color(1.0f, 1.0f, 1.0f, 1.0f);
    graphics.vsync(false);

    Input_Manager im{};

    Game asteroids{graphics, im};
    asteroids.run();

    return 0;
}
