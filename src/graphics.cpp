#include "graphics.hpp"

#include <glad/glad.h>
#include <SDL2/SDL.h>

#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <array>

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
    _circle_shader = std::make_unique<Shader>("res/shaders/circle_vert.glsl", "res/shaders/circle_frag.glsl");
    _line_shader = std::make_unique<Shader>("res/shaders/line_vert.glsl", "res/shaders/line_frag.glsl");

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
    _circle_shader.reset();
    _line_shader.reset();

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
    _settings.width = w;
    _settings.height = h;
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

void Graphics::wireframe(bool wireframe)
{ 
    if (wireframe) {
        GL_CALL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)); 
    }
    else {
        GL_CALL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL)); 
    }
}

// ======================================================================= Drawing functions =============================================================
// TODO optimize drawing renderer. Currently we create and destroy vao/vbo each frame and call drawCall on every object

// I use this only for triangle
// probably will rewrite renderer for
// the N-th time xD
struct Triangle_Vertex {
    glm::vec2 pos;
    glm::vec4 color;
};

struct Polygon_Vertex {
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
    GL_CALL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Triangle_Vertex), 0));

    GL_CALL(glEnableVertexAttribArray(1));
    GL_CALL(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Triangle_Vertex), (const void*) sizeof(Triangle_Vertex::pos)));

    _triangle_shader->bind();
    _triangle_shader->set_mat4("u_mvp", _projection*transform);
    GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 3));
}

void Graphics::draw_rect(glm::vec2 pos, glm::vec2 size, glm::vec4 color)
{
    
    uint32_t vao;
    GL_CALL(glCreateVertexArrays(1, &vao));
    GL_CALL(glBindVertexArray(vao));

    // use poly vertex for quad
    std::vector<Polygon_Vertex> vertex_data {
        {{pos.x,        pos.y-size.y}, color},
        {{pos.x,        pos.y       }, color},
        {{pos.x+size.x, pos.y       }, color},
        {{pos.x+size.x, pos.y-size.y}, color}
    };

    std::array<uint32_t, 6> index_data {0,1,2, 0,2,3};

    uint32_t vbo;
    GL_CALL(glCreateBuffers(1, &vbo));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(Polygon_Vertex)*vertex_data.size(), vertex_data.data(), GL_STATIC_DRAW));

    uint32_t ibo;
    GL_CALL(glCreateBuffers(1, &ibo));
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
    GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t)*index_data.size(), index_data.data(), GL_STATIC_DRAW));

    GL_CALL(glEnableVertexAttribArray(0));
    GL_CALL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Polygon_Vertex), 0));

    GL_CALL(glEnableVertexAttribArray(1));
    GL_CALL(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Polygon_Vertex), (const void*) sizeof(Polygon_Vertex::pos)));
    
    _triangle_shader->bind();
    _triangle_shader->set_mat4("u_mvp", _projection);
    GL_CALL(glDrawElements(GL_TRIANGLES, index_data.size(), GL_UNSIGNED_INT, 0));
}


// draw polygon without transform.
// points are in world space
void Graphics::draw_polygon(const std::vector<glm::vec2>& points, glm::vec4 color)
{
    PERIA_ASSERT(points.size() >= 3, "draw_polygon() needs at least 3 points");
    if (points.size() < 3) return;

    uint32_t vao;
    GL_CALL(glCreateVertexArrays(1, &vao));
    GL_CALL(glBindVertexArray(vao));

    std::vector<Polygon_Vertex> vertex_data(points.size());
    for (std::size_t i{}; i<vertex_data.size(); ++i) {
        vertex_data[i].pos = points[i];
        vertex_data[i].color = color;
    }

    std::vector<uint32_t> index_data;
    index_data.reserve((points.size()-2)*3);

    for (std::size_t i=1; i<=points.size()-2; ++i) {
        index_data.push_back(0); // 0-th point is the start
        index_data.push_back(i);
        index_data.push_back(i+1);
    }
    
    uint32_t vbo;
    GL_CALL(glCreateBuffers(1, &vbo));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(Polygon_Vertex)*vertex_data.size(), vertex_data.data(), GL_STATIC_DRAW));

    uint32_t ibo;
    GL_CALL(glCreateBuffers(1, &ibo));
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
    GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t)*index_data.size(), index_data.data(), GL_STATIC_DRAW));


    GL_CALL(glEnableVertexAttribArray(0));
    GL_CALL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Polygon_Vertex), 0));

    GL_CALL(glEnableVertexAttribArray(1));
    GL_CALL(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Polygon_Vertex), (const void*) sizeof(Polygon_Vertex::pos)));

    // current implementation of triangle shader works for polygons.
    // we just create polygon with triangles, using indexed draw call
    _triangle_shader->bind();
    // since we pass points in world space. No need for model matrix
    _triangle_shader->set_mat4("u_mvp", _projection);

    GL_CALL(glDrawElements(GL_TRIANGLES, index_data.size(), GL_UNSIGNED_INT, 0)); // indexed draw with triangles
}

// draw polygon with transform.
// points are normalized
void Graphics::draw_polygon(const std::vector<glm::vec2>& points, 
                            const glm::mat4& transform, glm::vec4 color)
{
    PERIA_ASSERT(points.size() >= 3, "draw_polygon() needs at least 3 points");
    if (points.size() < 3) return;

    uint32_t vao;
    GL_CALL(glCreateVertexArrays(1, &vao));
    GL_CALL(glBindVertexArray(vao));

    std::vector<Polygon_Vertex> vertex_data(points.size());
    for (std::size_t i{}; i<vertex_data.size(); ++i) {
        vertex_data[i].pos = points[i];
        vertex_data[i].color = color;
    }

    std::vector<uint32_t> index_data;
    index_data.reserve((points.size()-2)*3);

    for (std::size_t i=1; i<=points.size()-2; ++i) {
        index_data.push_back(0); // 0-th point is the start
        index_data.push_back(i);
        index_data.push_back(i+1);
    }
    
    uint32_t vbo;
    GL_CALL(glCreateBuffers(1, &vbo));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(Polygon_Vertex)*vertex_data.size(), vertex_data.data(), GL_STATIC_DRAW));

    uint32_t ibo;
    GL_CALL(glCreateBuffers(1, &ibo));
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
    GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t)*index_data.size(), index_data.data(), GL_STATIC_DRAW));


    GL_CALL(glEnableVertexAttribArray(0));
    GL_CALL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Polygon_Vertex), 0));

    GL_CALL(glEnableVertexAttribArray(1));
    GL_CALL(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Polygon_Vertex), (const void*) sizeof(Polygon_Vertex::pos)));

    // current implementation of triangle shader works for polygons.
    // we just create polygon with triangles, using indexed draw call
    _triangle_shader->bind();
    // since we pass points in world space. No need for model matrix
    _triangle_shader->set_mat4("u_mvp", _projection*transform);

    GL_CALL(glDrawElements(GL_TRIANGLES, index_data.size(), GL_UNSIGNED_INT, 0)); // indexed draw with triangles
}

void Graphics::draw_circle(glm::vec2 center, float radius, glm::vec4 color)
{
    uint32_t vao;
    GL_CALL(glCreateVertexArrays(1, &vao));
    GL_CALL(glBindVertexArray(vao));

    // use poly vertex for quad
    std::vector<Polygon_Vertex> vertex_data {
        {{center.x-radius, center.y-radius}, color},
        {{center.x-radius, center.y+radius}, color},
        {{center.x+radius, center.y+radius}, color},
        {{center.x+radius, center.y-radius}, color},
    };

    std::array<uint32_t, 6> index_data {0,1,2, 0,2,3};

    uint32_t vbo;
    GL_CALL(glCreateBuffers(1, &vbo));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(Polygon_Vertex)*vertex_data.size(), vertex_data.data(), GL_STATIC_DRAW));

    uint32_t ibo;
    GL_CALL(glCreateBuffers(1, &ibo));
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
    GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t)*index_data.size(), index_data.data(), GL_STATIC_DRAW));

    GL_CALL(glEnableVertexAttribArray(0));
    GL_CALL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Polygon_Vertex), 0));

    GL_CALL(glEnableVertexAttribArray(1));
    GL_CALL(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Polygon_Vertex), (const void*) sizeof(Polygon_Vertex::pos)));
    
    _circle_shader->bind();
    _circle_shader->set_mat4("u_mvp", _projection);
    _circle_shader->set_vec2("u_center", center);
    _circle_shader->set_float("u_radius", radius);
    GL_CALL(glDrawElements(GL_TRIANGLES, index_data.size(), GL_UNSIGNED_INT, 0));
}

void Graphics::draw_line(glm::vec2 p1, glm::vec2 p2, glm::vec4 color)
{
    uint32_t vao;
    GL_CALL(glCreateVertexArrays(1, &vao));
    GL_CALL(glBindVertexArray(vao));

    std::vector<float> vertex_data {
        p1.x, p1.y,
        p2.x, p2.y
    };

    uint32_t vbo;
    GL_CALL(glCreateBuffers(1, &vbo));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vertex_data.size(), vertex_data.data(), GL_STATIC_DRAW));
    
    GL_CALL(glEnableVertexAttribArray(0));
    GL_CALL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*2, 0));

    _line_shader->bind();
    _line_shader->set_mat4("u_mvp", _projection);
    _line_shader->set_vec4("u_color", color);
    GL_CALL(glDrawArrays(GL_LINES, 0, 2));
}

