#pragma once

#include <utility>
#include <string>
#include <memory>
#include <vector>

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>

// forward declare
typedef struct SDL_Window SDL_Window;
typedef void* SDL_GLContext;
class Shader;

struct Window_Settings {
    std::string title;
    int width;
    int height;
    bool fullscreen;
    bool resizable;
    Window_Settings()
        :title{"default title"}, width{800}, height{600}, fullscreen{false}, resizable{false}
    {}
    Window_Settings(const std::string& title_, int width_, int height_, 
                    bool fullscreen_, bool resizable_)
        :title{title_}, width{width_}, height{height_}, fullscreen{fullscreen_}, resizable{resizable_}
    {}
};

class Graphics {
public:
    Graphics(const Window_Settings& settings);
    ~Graphics();

    // color range [0.0f - 1.0f]
    void set_clear_color(float r, float g, float b, float a);
    void clear_buffer();
    void swap_buffers();

    // SDL Window releated functions
    void set_window_size(int w, int h);
    std::pair<int,int> get_window_size() const;

    void set_window_title(const std::string& title);
    std::string get_window_title() const;

    void set_window_resizable(bool resizable);

    void toggle_fullscreen();
    bool is_fullscreen() const;

    void vsync(bool vsync);
    void wireframe(bool wireframe);

    // drawing functions
    void draw_triangle(const glm::mat4& transform, glm::vec4 color);
    void draw_rect(glm::vec2 pos, glm::vec2 size, glm::vec4 color);
    void draw_polygon(const std::vector<glm::vec2>& poly_points, glm::vec4 color);
    void draw_polygon(const std::vector<glm::vec2>& points, 
                      const glm::mat4& transform, glm::vec4 color);
    void draw_circle(glm::vec2 center, float radius, glm::vec4 color);
    void draw_line(glm::vec2 p1, glm::vec2 p2, glm::vec4 color);

private:
    void cleanup();

    void set_viewport();
private:
    SDL_Window* _window;
    SDL_GLContext _context;
    Window_Settings _settings;

    float _r, _g, _b, _a;
    
    // orthographic projection
    glm::mat4 _projection;

    // shaders
    std::unique_ptr<Shader> _triangle_shader;
    std::unique_ptr<Shader> _circle_shader;
    std::unique_ptr<Shader> _line_shader;

public:
    Graphics(const Graphics&) = delete;
    Graphics& operator=(const Graphics&) = delete;
    Graphics(Graphics&&) = delete;
    Graphics& operator=(Graphics&&) = delete;
};
