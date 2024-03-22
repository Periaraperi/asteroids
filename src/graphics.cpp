#include "graphics.hpp"

#include <glad/glad.h>
#include <SDL2/SDL.h>

#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include "opengl_errors.hpp"
#include "peria_logger.hpp"
#include "shader.hpp"

Graphics::Graphics(const Window_Settings& settings)
    :_window{nullptr}, _context{nullptr}, 
    _settings{settings},
    _projection{glm::mat4(1.0f)}
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        PERIA_LOG(SDL_GetError());
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
        PERIA_LOG(SDL_GetError());
        cleanup();
        std::exit(EXIT_FAILURE);
    }

    _context = SDL_GL_CreateContext(_window);
    if (_context == nullptr) {
        PERIA_LOG(SDL_GetError());
        cleanup();
        std::exit(EXIT_FAILURE);
    }

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        PERIA_LOG(SDL_GetError());
        
        cleanup();
        std::exit(EXIT_FAILURE);
    }

    set_viewport();

    // vsync on by default
    vsync(true);

    _triangle_shader = std::make_unique<Shader>("res/shaders/tri_vert.glsl", "res/shaders/tri_frag.glsl");
    PERIA_LOG("Graphics ctor()");
}

Graphics::~Graphics()
{
    cleanup();
    PERIA_LOG("Graphics dtor()");
}

void Graphics::cleanup()
{
    // we want custom order for deletion, hence .reset()
    _triangle_shader.reset();

    SDL_GL_DeleteContext(_context);

    SDL_DestroyWindow(_window);

    SDL_Quit();
}

void Graphics::set_viewport()
{
    PERIA_LOG("Setting Viewport");
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
        if (SDL_SetWindowFullscreen(_window, SDL_WINDOW_FULLSCREEN) != 0) {
            PERIA_LOG(SDL_GetError());
        }
    }
    else {
        if (SDL_SetWindowFullscreen(_window, 0) != 0) {
            PERIA_LOG(SDL_GetError());
        }
    }
}

bool Graphics::is_fullscreen() const
{ return _settings.fullscreen; }

void Graphics::vsync(bool vsync)
{ SDL_GL_SetSwapInterval((vsync) ? 1 : 0); }

// ======================================================================= Drawing functions =============================================================

struct Triangle_Vertex {
    glm::vec2 pos;
    glm::vec4 color;
};

void Graphics::draw_triangle(const glm::mat4& transform, glm::vec4 color)
{
    uint32_t vao;
    GL_CALL(glCreateVertexArrays(1, &vao));
    GL_CALL(glBindVertexArray(vao));
    
    std::vector<Triangle_Vertex> vertex_data {
        {{-0.5f, -0.5f}, color},
        {{ 0.5f, -0.5f}, color}, 
        {{ 0.0f,  1.0f}, color}
    };
    
    uint32_t vbo;
    GL_CALL(glCreateBuffers(1, &vbo));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle_Vertex)*vertex_data.size(), vertex_data.data(), GL_STATIC_DRAW));

    GL_CALL(glEnableVertexAttribArray(0));
    GL_CALL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_data), 0));

    GL_CALL(glEnableVertexAttribArray(1));
    GL_CALL(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(vertex_data), (const void*) sizeof(Triangle_Vertex::pos)));

    _triangle_shader->bind();
    _triangle_shader->set_mat4("u_mvp", _projection*transform);
    GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 3));
}
