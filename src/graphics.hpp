#pragma once

#include <utility>
#include <string>
#include <memory>
#include <array>

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "vertex_buffer.hpp"
#include "framebuffer.hpp"

// forward declare
typedef struct SDL_Window SDL_Window;
typedef void* SDL_GLContext;

class Vertex_Array;
class Index_Buffer;
class Shader;
class Texture;

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

struct Screen_Vertex {
    glm::vec2 pos;
    glm::vec2 tex_coord;
};

struct Simple_Vertex {
    glm::vec2 pos;
    glm::vec4 color;
};

struct Rect_Vertex {
    glm::vec2 pos;
    glm::vec2 tex_coord;
    glm::vec4 color;
};

struct Circle_Vertex {
    glm::vec2 pos;
    glm::vec2 center;
    glm::vec4 color;
    float radius;
};

struct Glyph {
    long advance;
    glm::ivec2 size;
    glm::ivec2 bearing;

    // offset in atlas
    int offset_x; 
    int offset_y;
};

struct Font_Atlas_Data {
    std::unique_ptr<Texture> atlas;
    glm::vec2 atlas_size;
};

class Graphics {
public:
    Graphics(const Window_Settings& settings);
    ~Graphics();

    // color range [0.0f - 1.0f]
    void set_clear_color(float r, float g, float b, float a);
    void clear_buffer();
    void swap_buffers();
    void flush();

    // fbo stuff
    void bind_fbo_multisampled()
    { _fbo_multisampled->bind(); }

    void render_to_screen();

    // SDL Window related functions
    void set_window_size(int w, int h);
    std::pair<int,int> get_window_size() const;

    void set_window_title(const std::string& title);
    std::string get_window_title() const;

    void set_window_resizable(bool resizable);

    void toggle_fullscreen();
    bool is_fullscreen() const;

    void vsync(bool vsync);
    int get_vsync() const;

    [[nodiscard]]
    const std::string& get_executable_path() const
    { return _executable_path; }

    void wireframe(bool wireframe);

    // Drawing functions, these function just batch data. I.E add vertex info to big buffer.
    // Draw calls happen when we call flush on every frame.
    
    void draw_triangle(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, glm::vec4 color);
                                                                                   
    void draw_rect(glm::vec2 pos, glm::vec2 size, glm::vec4 color);

    void draw_polygon(const std::vector<glm::vec2>& poly_points, glm::vec4 color);

    void draw_circle(glm::vec2 center, float radius, glm::vec4 color);

    void draw_line(glm::vec2 p1, glm::vec2 p2, glm::vec4 color);

    void draw_text(const std::string& text, glm::vec2 pos, glm::vec3 color, int32_t font_size, float scale=1.0f);

private:
    void cleanup();

    void set_window_viewport();

    // render functions that do actual draw calls on batched data

    void render_triangles();

    void render_rects();

    void render_circles();

    void render_text();

private:
    SDL_Window* _window;
    SDL_GLContext _context;
    FT_Library _ft;
    Window_Settings _settings;
    std::string _executable_path;

    float _r, _g, _b, _a;
    
    // orthographic projection
    glm::mat4 _projection;
    glm::mat4 _game_world_projection;

    std::unique_ptr<Shader> _triangle_shader;
    std::unique_ptr<Shader> _circle_shader;
    std::unique_ptr<Shader> _line_shader;
    std::unique_ptr<Shader> _text_shader;
    std::unique_ptr<Shader> _texture_shader;

    std::vector<Font_Atlas_Data> _font_atlases;
    std::array<bool, 81> _font_size_loaded{};
    std::vector<std::array<Glyph, 128>> _font_structure;

    std::unique_ptr<Frame_Buffer> _fbo;
    std::unique_ptr<Frame_Buffer> _fbo_multisampled;

    // vao, vbo, ibo information for batching

    std::unique_ptr<Vertex_Array> _triangle_batch_vao;
    std::unique_ptr<Vertex_Buffer<Simple_Vertex>> _triangle_batch_vbo;

    std::unique_ptr<Vertex_Array> _circle_batch_vao;
    std::unique_ptr<Vertex_Buffer<Circle_Vertex>> _circle_batch_vbo;

    std::unique_ptr<Vertex_Array> _rect_batch_vao;
    std::unique_ptr<Vertex_Buffer<Simple_Vertex>> _rect_batch_vbo;

    std::vector<std::unique_ptr<Vertex_Array>> _text_vaos;
    std::vector<std::unique_ptr<Vertex_Buffer<Rect_Vertex>>> _text_vbos;

    std::unique_ptr<Index_Buffer> _ibo;

    std::unique_ptr<Vertex_Array> _screen_vao;
    std::unique_ptr<Vertex_Buffer<Screen_Vertex>> _screen_vbo;

    void init_circle_batch_data();
    void init_triangle_batch_data();
    void init_rect_batch_data();

    void load_font(const char* rel_path, int32_t font_size);

    std::string _game_font_path = "res/iosevka-regular.ttf";

public:
    Graphics(const Graphics&) = delete;
    Graphics& operator=(const Graphics&) = delete;
    Graphics(Graphics&&) = delete;
    Graphics& operator=(Graphics&&) = delete;
};
