#include "graphics.hpp"

#include <glad/glad.h>
#include <SDL2/SDL.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <glm/gtc/matrix_transform.hpp>

#include <array>

#include "vertex_array.hpp"
#include "index_buffer.hpp"

#include "shader.hpp"
#include "texture.hpp"
#include "physics.hpp"

constexpr int MAX_TRIANGLE_COUNT = 4096*2; // this many triangles per batch
constexpr int MAX_RECT_COUNT = 4096;
constexpr int MAX_CIRCLE_COUNT = 4096;

void Graphics::init_triangle_batch_data()
{
    _triangle_batch_vao = std::make_unique<Vertex_Array>();
    _triangle_batch_vbo = std::make_unique<Vertex_Buffer<Simple_Vertex>>(sizeof(Simple_Vertex)*MAX_TRIANGLE_COUNT*3);
    _ibo->bind();

    // position
    _triangle_batch_vao->add_attribute(2, GL_FLOAT, false, sizeof(Simple_Vertex));
    // color
    _triangle_batch_vao->add_attribute(4, GL_FLOAT, false, sizeof(Simple_Vertex));

    _triangle_batch_vao->set_layout();

    PERIA_LOG("INIT TRIANGLE BATCH DATA");
}

void Graphics::init_rect_batch_data()
{
    _rect_batch_vao = std::make_unique<Vertex_Array>();
    _rect_batch_vbo = std::make_unique<Vertex_Buffer<Simple_Vertex>>(sizeof(Simple_Vertex)*MAX_RECT_COUNT*4);
    _ibo->bind(); // reuse 1 ibo
    
    // pos
    _rect_batch_vao->add_attribute(2, GL_FLOAT, false, sizeof(Simple_Vertex));
    // color
    _rect_batch_vao->add_attribute(4, GL_FLOAT, false, sizeof(Simple_Vertex));
    _rect_batch_vao->set_layout();

    PERIA_LOG("INIT RECT BATCH DATA");
}

void Graphics::init_circle_batch_data()
{
    _circle_batch_vao = std::make_unique<Vertex_Array>();
    _circle_batch_vbo = std::make_unique<Vertex_Buffer<Circle_Vertex>>(sizeof(Circle_Vertex)*MAX_CIRCLE_COUNT*4);
    _ibo->bind(); // reuse 1 ibo
    
    // bounding box pos for shader
    _circle_batch_vao->add_attribute(2, GL_FLOAT, false, sizeof(Circle_Vertex));
    // center pos
    _circle_batch_vao->add_attribute(2, GL_FLOAT, false, sizeof(Circle_Vertex));
    // color
    _circle_batch_vao->add_attribute(4, GL_FLOAT, false, sizeof(Circle_Vertex));
    // radius
    _circle_batch_vao->add_attribute(1, GL_FLOAT, false, sizeof(Circle_Vertex));
    _circle_batch_vao->set_layout();

    PERIA_LOG("INIT CIRCLE BATCH DATA");
}

Graphics::Graphics(const Window_Settings& settings)
    :_window{nullptr}, _context{nullptr}, 
    _settings{settings},
    _projection{glm::mat4{1.0f}},
    _game_world_projection{glm::ortho(0.0f, static_cast<float>(1600.0f), 
                             0.0f, static_cast<float>(900.0f),
                             -1.0f, 1.0f)}
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

    // vsync on by default
    vsync(true);

    // enable blending
    GL_CALL(glEnable(GL_BLEND));
    GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    _triangle_shader = std::make_unique<Shader>("res/shaders/tri_vert.glsl", "res/shaders/tri_frag.glsl");
    _circle_shader = std::make_unique<Shader>("res/shaders/circle_vert.glsl", "res/shaders/circle_frag.glsl");
    _line_shader = std::make_unique<Shader>("res/shaders/line_vert.glsl", "res/shaders/line_frag.glsl");
    _text_shader = std::make_unique<Shader>("res/shaders/text_vert.glsl", "res/shaders/text_frag.glsl");
    _texture_shader = std::make_unique<Shader>("res/shaders/texture_vert.glsl", "res/shaders/texture_frag.glsl");

    // 1600 900 is game world
    _fbo = std::make_unique<Frame_Buffer>(1600, 900, Frame_Buffer::Frame_Buffer_Type::REGULAR);
    _fbo_multisampled = std::make_unique<Frame_Buffer>(1600, 900, Frame_Buffer::Frame_Buffer_Type::MULTI_SAMPLE);
    _texture_shader->bind();
    _texture_shader->set_int("u_texture", 0);
    _texture_shader->unbind();

    set_window_viewport(); // actual window viewport

    // general ibo here, unbind and bind on each setup of vao
    // below vaos share one ibo
    _ibo = std::make_unique<Index_Buffer>(4*6*MAX_CIRCLE_COUNT);
    _ibo->unbind();

    init_triangle_batch_data();

    init_rect_batch_data();

    init_circle_batch_data();

    //if (0)
    { // STILL EXPERIMENTING. Convert to SDF rendering later for better flexibility
        // font related stuff here
        FT_Library ft;
        if (FT_Init_FreeType(&ft) != 0) {
            PERIA_LOG("Failed to load freetype lib");
            std::exit(EXIT_FAILURE);
        }

        FT_Face face;
        if (FT_New_Face(ft, "./res/iosevka-regular.ttf", 0, &face) != 0) {
            PERIA_LOG("Failed to load font face\n", "Filepath: './res/iosevka-regular.ttf'");
            std::exit(EXIT_FAILURE);
        }

        if (FT_Set_Pixel_Sizes(face, 0, 48) != 0) {
            PERIA_LOG("Failed to set pixel size");
            std::exit(EXIT_FAILURE);
        }

        // hardcode for now
        _text_atlas_size = {512, 512};
        _text_atlas = std::make_unique<Texture>(_text_atlas_size.x, _text_atlas_size.y, GL_RED, GL_RED);

        int32_t xoff = 0;
        int32_t yoff = 0;
        int32_t max_y = 0; // dimensions are positive so assume min is 0

        for (uint8_t ch=32; ch<127; ++ch) {
            if (FT_Load_Char(face, ch, FT_LOAD_RENDER) != 0) {
                PERIA_LOG("Failed to FT_Load_Char() on char ", ch);
                std::exit(EXIT_FAILURE); // MUST LOAD ALL GLYPHS
            }

            _glyphs[ch] = {
                face->glyph->advance.x,
                {face->glyph->bitmap.width, face->glyph->bitmap.rows},
                {face->glyph->bitmap_left, face->glyph->bitmap_top},
                xoff, yoff // offset inside texture atlas
            };

            const auto& glyph = _glyphs[ch];
            max_y = std::max(max_y, glyph.size.y);

            PERIA_LOG(ch," info: ");
            PERIA_LOG(glyph.advance, " ", glyph.size.x , " "
                      , glyph.size.y , " " , glyph.bearing.x , " " , glyph.bearing.y , " "
                      , xoff , " " , yoff , '\n');
            //continue;
            auto glyph_width = face->glyph->bitmap.width;
            auto glyph_height = face->glyph->bitmap.rows;

            std::vector<uint8_t> reversed(glyph_width*glyph_height);
            for (std::size_t i{}; i<glyph_height; ++i) {
                for (std::size_t j{}; j<glyph_width; ++j) {
                    reversed[glyph_width*(glyph_height-i-1)+j] = face->glyph->bitmap.buffer[glyph_width*i+j];
                }
            }
            
            _text_atlas->write_sub_texture(xoff, yoff, 
                    face->glyph->bitmap.width, face->glyph->bitmap.rows,
                    reversed.data());
            xoff += (glyph.advance >> 6);
            if ((xoff + (glyph.advance >> 6)) >= _text_atlas_size.x) {
                xoff = 0;
                yoff += max_y;
                max_y = 0;
            }
        }

        // cleanup
        if (FT_Done_Face(face) != 0) {
            PERIA_LOG("Failed on FT_Done_Face()");
            std::exit(EXIT_FAILURE);
        }
        if (FT_Done_FreeType(ft) != 0) {
            PERIA_LOG("Failed on FT_Done_Freetype()");
            std::exit(EXIT_FAILURE);
        }

        _text_vao = std::make_unique<Vertex_Array>();
        _text_vbo = std::make_unique<Vertex_Buffer<Rect_Vertex>>(sizeof(Rect_Vertex)*4*MAX_RECT_COUNT);
        
        _ibo->bind();

        // quad pos
        _text_vao->add_attribute(2, GL_FLOAT, false, sizeof(Rect_Vertex));
        // tex coords
        _text_vao->add_attribute(2, GL_FLOAT, false, sizeof(Rect_Vertex));
        // color
        _text_vao->add_attribute(4, GL_FLOAT, false, sizeof(Rect_Vertex));
        _text_vao->set_layout();
    }

    { // screen framebuffer
        _screen_vao = std::make_unique<Vertex_Array>();
        std::vector<Screen_Vertex> screen_quad_data {
            {{-0.5f, -0.5f}, {0.0f, 0.0f}},
            {{-0.5f, 0.5f}, {0.0f, 1.0f}},
            {{0.5f, 0.5f}, {1.0f, 1.0f}},

            {{-0.5f, -0.5f}, {0.0f, 0.0f}},
            {{0.5f, 0.5f}, {1.0f, 1.0f}},
            {{0.5f, -0.5f}, {1.0f, 0.0f}},
        };

        _screen_vbo = std::make_unique<Vertex_Buffer<Screen_Vertex>>(screen_quad_data);
        // quad pos
        _screen_vao->add_attribute(2, GL_FLOAT, false, sizeof(Screen_Vertex));
        // tex coords
        _screen_vao->add_attribute(2, GL_FLOAT, false, sizeof(Screen_Vertex));
        _screen_vao->set_layout();
        _screen_vao->unbind();
    }
    
    SDL_ShowCursor(SDL_DISABLE);

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
    // shaders before SDL
    _triangle_shader.reset();
    _circle_shader.reset();
    _line_shader.reset();
    _text_shader.reset();
    _texture_shader.reset();

	_ibo.reset(); // release ibo

	_circle_batch_vbo.reset();
	_rect_batch_vbo.reset();
	_triangle_batch_vbo.reset();
	_screen_vbo.reset();
	_text_vbo.reset();

	_circle_batch_vao.reset();
	_triangle_batch_vao.reset();
	_rect_batch_vao.reset();
	_text_vao.reset();
	_screen_vao.reset();

	_text_atlas.reset();

	_fbo.reset();
	_fbo_multisampled.reset();

    SDL_GL_DeleteContext(_context);

    SDL_DestroyWindow(_window);

    SDL_Quit();
}

void Graphics::set_window_viewport()
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

// will scale game world texture to screen by stretching in both directions
void Graphics::render_to_screen()
{
    Frame_Buffer::copy_to(_fbo_multisampled.get(), _fbo.get());
    // bind default frame buffer
    _fbo->unbind();
    _fbo_multisampled->unbind();
    GL_CALL(glViewport(0, 0, _settings.width, _settings.height));
    clear_buffer();
    _texture_shader->bind();

    glm::mat4 screen_quad_model = glm::translate(glm::mat4{1.0f}, glm::vec3{_settings.width*0.5f, _settings.height*0.5f, 0.0f})*
                                  glm::scale(glm::mat4{1.0f}, glm::vec3{_settings.width, _settings.height, 1.0f});
    _texture_shader->set_mat4("u_mvp", _projection*screen_quad_model);
    _screen_vao->bind();
    _fbo->bind_color_texture();
    GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 6));
}

void Graphics::swap_buffers()
{ SDL_GL_SwapWindow(_window); }

void Graphics::set_window_size(int w, int h)
{ 
    SDL_SetWindowSize(_window, w, h); 
    _settings.width = w;
    _settings.height = h;
    set_window_viewport();
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

int Graphics::get_vsync() const
{ return SDL_GL_GetSwapInterval(); }
    

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
    _line_shader->set_mat4("u_mvp", _game_world_projection);
    _line_shader->set_vec4("u_color", color);
    GL_CALL(glDrawArrays(GL_LINES, 0, 2));


    // clean
    GL_CALL(glDeleteBuffers(1, &vbo));
    GL_CALL(glDeleteVertexArrays(1, &vao));
}

// poly_points in world space
void Graphics::draw_polygon(const std::vector<glm::vec2>& poly_points, glm::vec4 color)
{
    PERIA_ASSERT(poly_points.size() >= 3, "poly must have at least 3 points");
    auto tris = peria::Polygon{poly_points}.triangulate(true);
    for (auto&& t:tris) {
        draw_triangle(t.points()[0], t.points()[1], t.points()[2], color);
    }
}

// triangle points in world position in clockwise order
void Graphics::draw_triangle(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, glm::vec4 color)
{
    _triangle_batch_vbo->add_data({p1, color});
    _triangle_batch_vbo->add_data({p2, color});
    _triangle_batch_vbo->add_data({p3, color});
}

// rect points in world position in clockwise order
// pos -> rect's top left corner coordinates
void Graphics::draw_rect(glm::vec2 pos, glm::vec2 size, glm::vec4 color)
{
    _rect_batch_vbo->add_data({{pos.x, pos.y-size.y},        color});
    _rect_batch_vbo->add_data({{pos.x, pos.y},               color});
    _rect_batch_vbo->add_data({{pos.x+size.x, pos.y},        color});
    _rect_batch_vbo->add_data({{pos.x+size.x, pos.y-size.y}, color});
}

// center and radius in world position
void Graphics::draw_circle(glm::vec2 center, float radius, glm::vec4 color)
{
    _circle_batch_vbo->add_data({{center.x-radius, center.y-radius}, center, color, radius});
    _circle_batch_vbo->add_data({{center.x-radius, center.y+radius}, center, color, radius});
    _circle_batch_vbo->add_data({{center.x+radius, center.y+radius}, center, color, radius});
    _circle_batch_vbo->add_data({{center.x+radius, center.y-radius}, center, color, radius});
}

std::array<glm::vec2, 4> tex_coords_tmp(int x, int y, int w, int h, glm::vec2 atlas_size)
{
    auto atlas_width = atlas_size.x;
    auto atlas_height = atlas_size.y;

    return {{
        {x/atlas_width, y/atlas_height}, // lower left
        {x/atlas_width, (y+h)/atlas_height}, // upper left
        {(x+w)/atlas_width, (y+h)/atlas_height}, // upper right
        {(x+w)/atlas_width, y/atlas_height}, // lower right
    }};
}

// adds vertex data to large buffer
// pos start at bottom left corner unlike other drawing routines
void Graphics::draw_text(const std::string& text, glm::vec2 pos,
                         glm::vec3 color, float scale /* = 1.0f*/)
{
    for (const auto& c:text) {
        auto glyph = _glyphs[c];
        float xpos = pos.x + glyph.bearing.x*scale;
        float ypos = pos.y - (glyph.size.y - glyph.bearing.y)*scale;

        float w = glyph.size.x*scale;
        float h = glyph.size.y*scale;

        auto tex_coords = tex_coords_tmp(glyph.offset_x, glyph.offset_y, glyph.size.x, glyph.size.y, _text_atlas_size);

        _text_vbo->add_data({{xpos,   ypos  }, {tex_coords[0].x, tex_coords[0].y}, {color.r, color.g, color.b, 1.0f}});
        _text_vbo->add_data({{xpos,   ypos+h}, {tex_coords[1].x, tex_coords[1].y}, {color.r, color.g, color.b, 1.0f}});
        _text_vbo->add_data({{xpos+w, ypos+h}, {tex_coords[2].x, tex_coords[2].y}, {color.r, color.g, color.b, 1.0f}});
        _text_vbo->add_data({{xpos+w, ypos  }, {tex_coords[3].x, tex_coords[3].y}, {color.r, color.g, color.b, 1.0f}});

        pos.x += (glyph.advance >> 6)*scale;
    }

}

// should be called on each frame only once before swapping buffers.
// makes actual GL draw calls on batched data
void Graphics::flush()
{
    render_triangles();
    render_rects();
    render_circles();
    render_text();
}

void Graphics::render_triangles()
{
    if (_triangle_batch_vbo->data_empty()) return;

    int count = _triangle_batch_vbo->data_size() / 3; // overall, this many triangles
    std::size_t offset = 0; // offset inside _data

    _triangle_batch_vao->bind();
    _triangle_batch_vbo->bind();
    _triangle_shader->bind();
    _triangle_shader->set_mat4("u_mvp", _game_world_projection);

    while (count > 0) {
        int c = 0; // count of triangles for each batch
        if (count >= MAX_TRIANGLE_COUNT) {
            c = MAX_TRIANGLE_COUNT;
        }
        else { // smaller remaining batch
            c = count;
        }
        _triangle_batch_vbo->set_subdata(0, offset, 3*c*sizeof(Simple_Vertex));
        GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 3*c));
        offset += 3*c;
        count -= c;
    }

    _triangle_batch_vbo->clear_data();
    _triangle_batch_vbo->unbind();
}

void Graphics::render_rects()
{
    if (_rect_batch_vbo->data_empty()) return;

    int count = _rect_batch_vbo->data_size() / 4; // overall, this many rects
    std::size_t offset = 0; // offset inside _data

    _rect_batch_vao->bind();
    _rect_batch_vbo->bind();
    // just reuse tri shader
    _triangle_shader->bind();
    _triangle_shader->set_mat4("u_mvp", _game_world_projection);
    
    while (count > 0) {
        int c = 0; // count of rects for each batch
        if (count >= MAX_RECT_COUNT) {
            c = MAX_RECT_COUNT;
        }
        else { // smaller remaining batch
            c = count;
        }

        _rect_batch_vbo->set_subdata(0, offset, 4*c*sizeof(Simple_Vertex));
        GL_CALL(glDrawElements(GL_TRIANGLES, c*6, GL_UNSIGNED_INT, nullptr));
        offset += 4*c;
        count -= c;
    }

    _rect_batch_vbo->clear_data();
    _rect_batch_vbo->unbind();
}

void Graphics::render_circles()
{
    if (_circle_batch_vbo->data_empty()) return;

    int count = _circle_batch_vbo->data_size() / 4; // overall, this many rects
    std::size_t offset = 0; // offset inside _data

    _circle_batch_vao->bind();
    _circle_batch_vbo->bind();

    _circle_shader->bind();
    _circle_shader->set_mat4("u_mvp", _game_world_projection);
    
    while (count > 0) {
        int c = 0; // count of rects for each batch
        if (count >= MAX_CIRCLE_COUNT) {
            c = MAX_CIRCLE_COUNT;
        }
        else { // smaller remaining batch
            c = count;
        }

        _circle_batch_vbo->set_subdata(0, offset, 4*c*sizeof(Circle_Vertex));
        GL_CALL(glDrawElements(GL_TRIANGLES, c*6, GL_UNSIGNED_INT, nullptr));
        offset += 4*c;
        count -= c;
    }

    _circle_batch_vbo->clear_data();
    _circle_batch_vbo->unbind();
}

void Graphics::render_text()
{
    if (_text_vbo->data_empty()) return;

    int count = _text_vbo->data_size() / 4; // overall, this many rects
    std::size_t offset = 0; // offset inside text _data

    _text_vao->bind();
    _text_vbo->bind();
    // just reuse tri shader
    _text_shader->bind();
    _text_shader->set_mat4("u_mvp", _game_world_projection);
    _text_atlas->bind();
    
    while (count > 0) {
        int c = 0; // count of rects for each batch
        if (count >= MAX_RECT_COUNT) {
            c = MAX_RECT_COUNT;
        }
        else { // smaller remaining batch
            c = count;
        }
        _text_vbo->set_subdata(0, offset, 4*c*sizeof(Rect_Vertex));
        GL_CALL(glDrawElements(GL_TRIANGLES, c*6, GL_UNSIGNED_INT, nullptr));
        offset += 4*c;
        count -= c;
    }

    _text_vbo->clear_data();
    _text_vbo->unbind();
}
