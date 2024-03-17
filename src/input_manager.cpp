#include "input_manager.hpp"

#include <SDL2/SDL.h>
#include <algorithm> // for std::copy
#include <iostream>

Input_Manager::Input_Manager()
    :_keyboard_state{nullptr}, 
    _prev_keyboard_state{nullptr}, 
    _key_length{0}
{
    // no need to call SDL_GetKeyboardState again, we pollevents in game loop which
    // pumps events, which updates keyboard_state array, we just maintain pointer to it
    _keyboard_state = SDL_GetKeyboardState(&_key_length);
    _prev_keyboard_state = new uint8_t[_key_length];
    std::copy(_keyboard_state, _keyboard_state+_key_length, _prev_keyboard_state);
    _mouse_state = SDL_GetMouseState(&_mouse_x, &_mouse_y);
    
    std::cerr << "Input Manager Init\n";
}

Input_Manager::~Input_Manager()
{
    std::cerr << "Input Manager Dtor()\n";
    delete[] _prev_keyboard_state; 
    _prev_keyboard_state = nullptr;
}

Input_Manager& Input_Manager::Instance() 
{ 
    static Input_Manager _instance;
    return _instance;
}

void Input_Manager::update_prev_state()
{
    std::copy(_keyboard_state, _keyboard_state+_key_length, _prev_keyboard_state);
    _prev_mouse_state = _mouse_state;
}

void Input_Manager::update_mouse()
{
    _mouse_state = SDL_GetMouseState(&_mouse_x,&_mouse_y);
}

std::pair<int, int> Input_Manager::get_mouse()
{
    return {_mouse_x, _mouse_y};
}

uint32_t Input_Manager::get_mask(Mouse_Button btn)
{
    uint32_t mask = 0;
    switch (btn) {
        case Mouse_Button::LEFT:
            mask = SDL_BUTTON_LMASK;
            break;
        case Mouse_Button::MID:
            mask = SDL_BUTTON_MMASK;
            break;
        case Mouse_Button::RIGHT:
            mask = SDL_BUTTON_RMASK;
            break;
    }
    return mask;
}

bool Input_Manager::key_pressed(SDL_Scancode key)
{ return (_keyboard_state[key] && !_prev_keyboard_state[key]); }

bool Input_Manager::key_down(SDL_Scancode key)
{ return (_keyboard_state[key] && _prev_keyboard_state[key]); }

bool Input_Manager::key_released(SDL_Scancode key)
{ return (!_keyboard_state[key] && _prev_keyboard_state[key]); }

bool Input_Manager::mouse_pressed(Mouse_Button btn)
{
    auto mask = get_mask(btn);
    return ((_mouse_state&mask)!=0 && (_prev_mouse_state&mask)==0);
}

bool Input_Manager::mouse_down(Mouse_Button btn)
{
    auto mask = get_mask(btn);
    return ((_mouse_state&mask)!=0 && (_prev_mouse_state&mask)!=0);
}

bool Input_Manager::mouse_released(Mouse_Button btn)
{
    auto mask = get_mask(btn);
    return ((_mouse_state&mask)==0 && (_prev_mouse_state&mask)!=0);
}
