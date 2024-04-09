#include <SDL2/SDL.h>
#include <glm/gtc/matrix_transform.hpp>

#include "graphics.hpp"
#include "input_manager.hpp"
#include "exp.hpp"

int main()
{
    Graphics graphics{Window_Settings{"asteroids", 1600, 900, false, false}};
    graphics.set_clear_color(0, 0.5f, 0.5f, 1.0f);
    graphics.vsync(false);

    Input_Manager im{};

    Exp exp{graphics, im};
    exp.run();

    return 0;
}
