#pragma once

#include <cstdint>
#include <utility>
#include <SDL2/SDL_scancode.h>

enum class Mouse_Button {
    LEFT = 0,
    MID,
    RIGHT
};

class Input_Manager {
public:
    Input_Manager();
    ~Input_Manager();

    bool key_pressed(SDL_Scancode key);
    bool key_down(SDL_Scancode key);
    bool key_released(SDL_Scancode key);
 
    bool mouse_pressed(Mouse_Button btn);
    bool mouse_down(Mouse_Button btn);
    bool mouse_released(Mouse_Button btn);

    std::pair<int, int> get_mouse();

    void update_prev_state();
    void update_mouse();
private:

    uint32_t get_mask(Mouse_Button btn);
private:

    const uint8_t* _keyboard_state;
    uint8_t* _prev_keyboard_state;
    int _key_length;

    uint32_t _mouse_state;
    uint32_t _prev_mouse_state;
    int _mouse_x, _mouse_y;

public:
    // disable copying and moving
    Input_Manager(const Input_Manager&) = delete;
    Input_Manager operator=(const Input_Manager&) = delete;
    Input_Manager(Input_Manager&&) = delete;
    Input_Manager operator=(Input_Manager&&) = delete;
};
