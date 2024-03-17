#include "graphics.hpp"

#include <glad/glad.h>
#include <SDL2/SDL.h>

#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

#include "opengl_errors.hpp"

Graphics::Graphics()
    :_window{nullptr}, _context{nullptr}, 
    _settings{},
    _projection{glm::mat4(1.0f)}
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << SDL_GetError() << '\n';
        std::exit(EXIT_FAILURE);
    }
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    const auto& s = _settings;
    auto fullscreen_flag = s.fullscreen ? SDL_WINDOW_FULLSCREEN : 0;
    auto resizable_flag = s.resizable ? SDL_WINDOW_RESIZABLE : 0;

    _window = SDL_CreateWindow(s.title.c_str(), 
                               SDL_WINDOWPOS_CENTERED, 
                               SDL_WINDOWPOS_CENTERED, 
                               s.width, s.height, 
                               SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | fullscreen_flag | resizable_flag);

    if (_window == nullptr) {
        std::cerr << SDL_GetError() << '\n';
        cleanup();
        std::exit(EXIT_FAILURE);
    }

    _context = SDL_GL_CreateContext(_window);
    if (_context == nullptr) {
        std::cerr << SDL_GetError() << '\n';
        cleanup();
        std::exit(EXIT_FAILURE);
    }

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        std::cerr << SDL_GetError() << '\n';
        
        cleanup();
        std::exit(EXIT_FAILURE);
    }

    set_viewport();

    // vsync on by default
    vsync(true);

    std::cerr << "Init Graphics\n";
}

Graphics::~Graphics()
{
    std::cerr << "Graphics Dtor()\n";
    cleanup();
}

void Graphics::cleanup()
{
    if (_context != nullptr) {
        SDL_GL_DeleteContext(_context);
        _context = nullptr;
    }

    if (_window != nullptr){
        SDL_DestroyWindow(_window);
        _window = nullptr;
    }

    SDL_Quit();
}

Graphics& Graphics::Instance()
{
    static Graphics instance;
    return instance;
}

void Graphics::set_viewport()
{
    std::cerr << "Setting Viewport\n";
    const auto& s = _settings;
    GL_CALL(glViewport(0, 0, s.width, s.height));
    _projection = glm::ortho(0.0f, static_cast<float>(s.width), 
                             0.0f, static_cast<float>(s.height),
                             -1.0f, 1.0f);
}

void Graphics::set_clear_color(float r, float g, float b, float a)
{
    _r = r;
    _g = g;
    _b = b;
    _a = a;
}

void Graphics::clear_buffer()
{
    GL_CALL(glClearColor(_r, _g, _b, _a));
    GL_CALL(glClear(GL_COLOR_BUFFER_BIT));
}

void Graphics::swap_buffers()
{ SDL_GL_SwapWindow(_window); }

void Graphics::set_window_size(int w, int h)
{ 
    SDL_SetWindowSize(_window, w, h); 
    set_viewport();
}

std::pair<int, int> Graphics::get_window_size() const
{ return {_settings.width, _settings.height}; }

void Graphics::set_window_title(const std::string& title)
{ 
    _settings.title = title;
    SDL_SetWindowTitle(_window, _settings.title.c_str());
}

std::string Graphics::get_window_title() const
{ return _settings.title; }

void Graphics::set_window_resizable(bool resizable)
{
    _settings.resizable = resizable;
    SDL_SetWindowResizable(_window, _settings.resizable ? SDL_TRUE : SDL_FALSE);
}

void Graphics::toggle_fullscreen()
{ 
    _settings.fullscreen = !_settings.fullscreen;
    if (_settings.fullscreen) {
        SDL_SetWindowFullscreen(_window, SDL_WINDOW_FULLSCREEN);
    }
    else {
        SDL_SetWindowFullscreen(_window, 0);
    }
}

bool Graphics::is_fullscreen() const
{ return _settings.fullscreen; }

void Graphics::vsync(bool vsync)
{ SDL_GL_SetSwapInterval((vsync) ? 1 : 0); }

