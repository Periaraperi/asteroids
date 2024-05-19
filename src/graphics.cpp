#include "graphics.hpp"

#include <glad/glad.h>
#include <SDL2/SDL.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <array>

#include "opengl_errors.hpp"
#include "peria_logger.hpp"
#include "shader.hpp"
#include "texture.hpp"

constexpr int MAX_TRIANGLE_COUNT = 4096; // this many triangles per batch
uint32_t triangle_batch_vao;
uint32_t triangle_batch_vbo; // dynamic buffer

constexpr int MAX_RECT_COUNT = 4096;
uint32_t rect_batch_vao;
uint32_t rect_batch_vbo; // dynamic buffer
uint32_t rect_batch_ibo; // index buffer for rects
std::vector<uint32_t> rect_indices; // store indices here

constexpr int MAX_CIRCLE_COUNT = 4096;
uint32_t circle_batch_vao;
uint32_t circle_batch_vbo; // dynamic buffer
uint32_t circle_batch_ibo; // index buffer for rects
std::vector<uint32_t> circle_indices; // store indices here

uint32_t text_vao;
uint32_t text_vbo;

void init_triangle_batch_data()
{
    GL_CALL(glCreateVertexArrays(1, &triangle_batch_vao));
    GL_CALL(glBindVertexArray(triangle_batch_vao));
    
    GL_CALL(glCreateBuffers(1, &triangle_batch_vbo));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, triangle_batch_vbo));
    // make buffer dynamic since we will update data frequently
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle_Vertex)*MAX_TRIANGLE_COUNT*3, nullptr, GL_DYNAMIC_DRAW));

    // position
    GL_CALL(glEnableVertexAttribArray(0));
    GL_CALL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Triangle_Vertex), (const void*)0));

    // color
    GL_CALL(glEnableVertexAttribArray(1));
    GL_CALL(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Triangle_Vertex), (const void*)sizeof(glm::vec2)));

    PERIA_LOG("INIT TRIANGLE BATCH DATA");
}

void init_rect_batch_data()
{
    GL_CALL(glCreateVertexArrays(1, &rect_batch_vao));
    GL_CALL(glBindVertexArray(rect_batch_vao));
    
    GL_CALL(glCreateBuffers(1, &rect_batch_vbo));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, rect_batch_vbo));
    // make buffer dynamic since we will update data frequently
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(Rect_Vertex)*MAX_RECT_COUNT*4, nullptr, GL_DYNAMIC_DRAW));

    // position
    GL_CALL(glEnableVertexAttribArray(0));
    GL_CALL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Rect_Vertex), (const void*)0));

    // color
    GL_CALL(glEnableVertexAttribArray(1));
    GL_CALL(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Rect_Vertex), (const void*)sizeof(glm::vec2)));

    // index buffer
    std::size_t count = 4*6*MAX_RECT_COUNT;
    rect_indices.reserve(count);
    for (std::size_t i{}; i<count; i += 4) {
        rect_indices.push_back(i);
        rect_indices.push_back(i+1);
        rect_indices.push_back(i+2);

        rect_indices.push_back(i);
        rect_indices.push_back(i+2);
        rect_indices.push_back(i+3);
    }
    
    GL_CALL(glCreateBuffers(1, &rect_batch_ibo));
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rect_batch_ibo));

    GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, rect_indices.size()*sizeof(uint32_t), rect_indices.data(), GL_STATIC_DRAW));

    PERIA_LOG("INIT RECT BATCH DATA");
}

void init_circle_batch_data()
{
    GL_CALL(glCreateVertexArrays(1, &circle_batch_vao));
    GL_CALL(glBindVertexArray(circle_batch_vao));
    
    GL_CALL(glCreateBuffers(1, &circle_batch_vbo));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, circle_batch_vbo));
    // make buffer dynamic since we will update data frequently
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(Circle_Vertex)*MAX_CIRCLE_COUNT*4, nullptr, GL_DYNAMIC_DRAW));

    std::size_t offset = 0;

    // position
    GL_CALL(glEnableVertexAttribArray(0));
    GL_CALL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Circle_Vertex), (const void*)offset));
    offset += sizeof(Circle_Vertex::pos);
    
    // center
    GL_CALL(glEnableVertexAttribArray(1));
    GL_CALL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Circle_Vertex), (const void*)offset));
    offset += sizeof(Circle_Vertex::center);

    // color
    GL_CALL(glEnableVertexAttribArray(2));
    GL_CALL(glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Circle_Vertex), (const void*)offset));
    offset += sizeof(Circle_Vertex::color);

    // radius
    GL_CALL(glEnableVertexAttribArray(3));
    GL_CALL(glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Circle_Vertex), (const void*)offset));
    offset += sizeof(Circle_Vertex::radius);

    // index buffer
    std::size_t count = 4*6*MAX_CIRCLE_COUNT;
    circle_indices.reserve(count);
    for (std::size_t i{}; i<count; i += 4) {
        circle_indices.push_back(i);
        circle_indices.push_back(i+1);
        circle_indices.push_back(i+2);

        circle_indices.push_back(i);
        circle_indices.push_back(i+2);
        circle_indices.push_back(i+3);
    }
    
    GL_CALL(glCreateBuffers(1, &circle_batch_ibo));
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, circle_batch_ibo));

    GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, circle_indices.size()*sizeof(uint32_t), circle_indices.data(), GL_STATIC_DRAW));

    PERIA_LOG("INIT CIRCLE BATCH DATA");
}

void clean_batch_data()
{
    GL_CALL(glDeleteBuffers(1, &triangle_batch_vbo));
    GL_CALL(glDeleteVertexArrays(1, &triangle_batch_vao));

    GL_CALL(glDeleteBuffers(1, &rect_batch_vbo));
    GL_CALL(glDeleteBuffers(1, &rect_batch_ibo));
    GL_CALL(glDeleteVertexArrays(1, &rect_batch_vao));

    GL_CALL(glDeleteBuffers(1, &circle_batch_vbo));
    GL_CALL(glDeleteBuffers(1, &circle_batch_ibo));
    GL_CALL(glDeleteVertexArrays(1, &circle_batch_vao));

    PERIA_LOG("CLEANED BATCH DATA");
}

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

    // enable blending
    GL_CALL(glEnable(GL_BLEND));
    GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    _triangle_shader = std::make_unique<Shader>("res/shaders/tri_vert.glsl", "res/shaders/tri_frag.glsl");
    _circle_shader = std::make_unique<Shader>("res/shaders/circle_vert.glsl", "res/shaders/circle_frag.glsl");
    _line_shader = std::make_unique<Shader>("res/shaders/line_vert.glsl", "res/shaders/line_frag.glsl");
    _text_shader = std::make_unique<Shader>("res/shaders/text_vert.glsl", "res/shaders/text_frag.glsl");
    _texture_shader = std::make_unique<Shader>("res/shaders/texture_vert.glsl", "res/shaders/texture_frag.glsl");

    _test_texture = std::make_unique<Texture>(80, 80);
    _test_texture->write_sub_texture_color(0, 0, 24, 24);

    // batching related stuff
    _triangles_vertices.reserve(MAX_TRIANGLE_COUNT*3);
    init_triangle_batch_data();

    _rects_vertices.reserve(MAX_RECT_COUNT*4);
    init_rect_batch_data();

    _circles_vertices.reserve(MAX_CIRCLE_COUNT*4);
    init_circle_batch_data();

    //if (0)
    {
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

        //TODO: dynamically calculate optimal powers of 2
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
                xoff, yoff
            };

            const auto& glyph = _glyphs[ch];
            max_y = std::max(max_y, glyph.size.y);

            std::cerr << ch << " info: ";
            std::cerr << glyph.advance << " " << glyph.size.x << " "
                      << glyph.size.y << " " << glyph.bearing.x << " " << glyph.bearing.y << " "
                      << xoff << " " << yoff << '\n';
            //continue;
            auto glyph_width = face->glyph->bitmap.width;
            auto glyph_height = face->glyph->bitmap.rows;

            std::vector<uint8_t> reversed(glyph_width*glyph_height);
            for (std::size_t i{}; i<glyph_height; ++i) {
                for (std::size_t j{}; j<glyph_width; ++j) {
                    reversed[glyph_width*(glyph_height-i-1)+j] = face->glyph->bitmap.buffer[glyph_width*i+j];
                    //reversed[glyph_width*i+j] = face->glyph->bitmap.buffer[glyph_width*i+j];
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

        PERIA_LOG("{ ", _glyphs['{'].advance, " | ", _glyphs['|'].advance, 
                " } ", _glyphs['}'].advance);

        // cleanup
        if (FT_Done_Face(face) != 0) {
            PERIA_LOG("Failed on FT_Done_Face()");
            std::exit(EXIT_FAILURE);
        }
        if (FT_Done_FreeType(ft) != 0) {
            PERIA_LOG("Failed on FT_Done_Freetype()");
            std::exit(EXIT_FAILURE);
        }


        // gl buffers here

        GL_CALL(glCreateVertexArrays(1, &text_vao));
        GL_CALL(glBindVertexArray(text_vao));
        
        GL_CALL(glCreateBuffers(1, &text_vbo));
        GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, text_vbo));
        GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(Temp_Vertex)*4*MAX_RECT_COUNT, nullptr, GL_DYNAMIC_DRAW));

        GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rect_batch_ibo)); // this works

        GL_CALL(glEnableVertexAttribArray(0));
        GL_CALL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Temp_Vertex), 0));

        GL_CALL(glEnableVertexAttribArray(1));
        GL_CALL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Temp_Vertex), (const void*)(2*sizeof(float))));

        GL_CALL(glEnableVertexAttribArray(2));
        GL_CALL(glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Temp_Vertex), (const void*)(4*sizeof(float))));
    }

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

    clean_batch_data();

    GL_CALL(glDeleteBuffers(1, &text_vbo));
    GL_CALL(glDeleteVertexArrays(1, &text_vao));

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

// TODO: Need to optimize line rendering and make it better
// will do for now, since I use this for debugging stuff
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


    // clean
    GL_CALL(glDeleteBuffers(1, &vbo));
    GL_CALL(glDeleteVertexArrays(1, &vao));
}

// poly_points in world space
void Graphics::draw_polygon(const std::vector<glm::vec2>& poly_points, glm::vec4 color)
{
    PERIA_ASSERT(poly_points.size() >= 3, "poly must have at least 3 points");
    for (std::size_t i=1; i<poly_points.size()-1; ++i) {
        _triangles_vertices.push_back({poly_points[0],   color});
        _triangles_vertices.push_back({poly_points[i],   color});
        _triangles_vertices.push_back({poly_points[i+1], color});
    }
}

// triangle points in world position in clockwise order
void Graphics::draw_triangle(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, glm::vec4 color)
{
    _triangles_vertices.push_back({p1, color});
    _triangles_vertices.push_back({p2, color});
    _triangles_vertices.push_back({p3, color});
}

// rect points in world position in clockwise order
// pos -> rect's top left corner coordinates
void Graphics::draw_rect(glm::vec2 pos, glm::vec2 size, glm::vec4 color)
{
    _rects_vertices.push_back({{pos.x, pos.y-size.y},        color});
    _rects_vertices.push_back({{pos.x, pos.y},               color});
    _rects_vertices.push_back({{pos.x+size.x, pos.y},        color});
    _rects_vertices.push_back({{pos.x+size.x, pos.y-size.y}, color});
}

//TEST SHIT
void Graphics::draw_rect(glm::vec2 pos, glm::vec2 size, glm::vec2 tex_coord)
{
    uint32_t vao;
    GL_CALL(glGenVertexArrays(1, &vao));
    GL_CALL(glBindVertexArray(vao));

    //const auto& glyph = _glyphs['('];
    //size.x = glyph.size.x;
    //size.y = glyph.size.y;

    //std::vector<Temp_Vertex> data {
    //    {{pos.x,        pos.y-size.y}, {glyph.offset_x/512.0f, glyph.offset_y/512.0f} /*{0.0f, 0.0f}*/},
    //    {{pos.x,        pos.y},        {glyph.offset_x/512.0f, (glyph.offset_y+glyph.size.y)/512.0f} /*{0.0f, 1.0f}*/},
    //    {{pos.x+size.x, pos.y},        {(glyph.offset_x+glyph.size.x)/512.0f, (glyph.offset_y+glyph.size.y)/512.0f} /*{1.0f, 1.0f}*/},
    //    {{pos.x+size.x, pos.y-size.y}, {(glyph.offset_x+glyph.size.x)/512.0f, glyph.offset_y/512.0f} /*{1.0f, 0.0f}*/},
    //};
    size = _text_atlas_size;
    std::vector<Temp_Vertex> data {
        {{pos.x,        pos.y-size.y}, {0.0f, 0.0f}, {0,0,0,1}},
        {{pos.x,        pos.y},        {0.0f, 1.0f}, {0,0,0,1}},
        {{pos.x+size.x, pos.y},        {1.0f, 1.0f}, {0,0,0,1}},
        {{pos.x+size.x, pos.y-size.y}, {1.0f, 0.0f}, {0,0,0,1}},
    };
    
    uint32_t vbo;
    GL_CALL(glGenBuffers(1, &vbo));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(Temp_Vertex)*data.size(), data.data(), GL_STATIC_DRAW));

    std::vector<uint32_t> indices {0,1,2, 0,2,3};
    uint32_t ibo;
    GL_CALL(glGenBuffers(1, &ibo));
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
    GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t)*indices.size(), indices.data(), GL_STATIC_DRAW));

    // position
    GL_CALL(glEnableVertexAttribArray(0));
    GL_CALL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Temp_Vertex), (const void*)0));

    // tex coords
    GL_CALL(glEnableVertexAttribArray(1));
    GL_CALL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Temp_Vertex), (const void*)sizeof(glm::vec2)));
    
    _texture_shader->bind();
    _texture_shader->set_mat4("u_mvp", _projection);
    _text_atlas->bind();
    GL_CALL(glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr));

    GL_CALL(glDeleteBuffers(1, &vbo));
    GL_CALL(glDeleteBuffers(1, &ibo));
    GL_CALL(glDeleteVertexArrays(1, &vao));
}

// center and radius in world position
void Graphics::draw_circle(glm::vec2 center, float radius, glm::vec4 color)
{
    _circles_vertices.push_back({{center.x-radius, center.y-radius}, center, color, radius});
    _circles_vertices.push_back({{center.x-radius, center.y+radius}, center, color, radius});
    _circles_vertices.push_back({{center.x+radius, center.y+radius}, center, color, radius});
    _circles_vertices.push_back({{center.x+radius, center.y-radius}, center, color, radius});
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

// switch to string_view later
// very inefficient, this is code from learnOpengl text rendering section
// want to experiment more with texts, currently remembering things and fucking around
void Graphics::draw_text(const std::string& text, glm::vec2 pos,
                         glm::vec3 color, float scale /* = 1.0f*/)
{
    for (const auto& c:text) {
        auto glyph = _glyphs[c];
        float xpos = pos.x + glyph.bearing.x*scale;
        float ypos = pos.y + (glyph.bearing.y)*scale;

        float w = glyph.size.x*scale;
        float h = glyph.size.y*scale;

        auto tex_coords = tex_coords_tmp(glyph.offset_x, glyph.offset_y, glyph.size.x, glyph.size.y, _text_atlas_size);

        // pos is already in world space
        //std::array<float, 6*4> verts {
        //    xpos,     ypos + h,  tex_coords[0].x, tex_coords[0].y, /*0.0f, 0.0f,*/
        //    xpos,     ypos,      tex_coords[1].x, tex_coords[1].y, /*0.0f, 1.0f,*/
        //    xpos + w, ypos,      tex_coords[2].x, tex_coords[2].y, /*1.0f, 1.0f,*/

        //    xpos,     ypos + h,  tex_coords[0].x, tex_coords[0].y, /*0.0f, 0.0f,*/
        //    xpos + w, ypos,      tex_coords[2].x, tex_coords[2].y, /*1.0f, 1.0f,*/
        //    xpos + w, ypos + h,  tex_coords[3].x, tex_coords[3].y /*1.0f, 0.0f */         
        //};

        _text_rects_vertices.push_back({{xpos,   ypos-h}, {tex_coords[0].x, tex_coords[0].y}, {color.r, color.g, color.b, 1.0f}});
        _text_rects_vertices.push_back({{xpos,   ypos  }, {tex_coords[1].x, tex_coords[1].y}, {color.r, color.g, color.b, 1.0f}});
        _text_rects_vertices.push_back({{xpos+w, ypos  }, {tex_coords[2].x, tex_coords[2].y}, {color.r, color.g, color.b, 1.0f}});
        _text_rects_vertices.push_back({{xpos+w, ypos-h}, {tex_coords[3].x, tex_coords[3].y}, {color.r, color.g, color.b, 1.0f}});

        //std::array<float, 6*4> verts {
        //    xpos,     ypos - h,  tex_coords[0].x, tex_coords[0].y, /*0.0f, 0.0f,*/
        //    xpos,     ypos,      tex_coords[1].x, tex_coords[1].y, /*0.0f, 1.0f,*/
        //    xpos + w, ypos,      tex_coords[2].x, tex_coords[2].y, /*1.0f, 1.0f,*/

        //    xpos,     ypos - h,  tex_coords[0].x, tex_coords[0].y, /*0.0f, 0.0f,*/
        //    xpos + w, ypos,      tex_coords[2].x, tex_coords[2].y, /*1.0f, 1.0f,*/
        //    xpos + w, ypos - h,  tex_coords[3].x, tex_coords[3].y /*1.0f, 0.0f */         
        //};

        //GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, text_vbo));
        //GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts.data()));
        //
        //GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 6));

        pos.x += (glyph.advance >> 6)*scale;
    }

}

// from text atlas
void Graphics::draw_text2(const std::string& text, glm::vec2 pos,
                          glm::vec3 color, float scale /* = 1.0f*/)
{
    GL_CALL(glBindVertexArray(text_vao));

    _text_shader->bind();
    _text_shader->set_vec3("u_color", color);
    _text_shader->set_mat4("u_mvp", _projection);
    _text_atlas->bind();

    for (const auto& c:text) {
        auto glyph = _glyphs[c];
        float xpos = pos.x + glyph.bearing.x*scale;
        float ypos = pos.y + (glyph.bearing.y)*scale;

        float w = glyph.size.x*scale;
        float h = glyph.size.y*scale;

        auto tex_coords = tex_coords_tmp(glyph.offset_x, glyph.offset_y, glyph.size.x, glyph.size.y, _text_atlas_size);

        // pos is already in world space
        //std::array<float, 6*4> verts {
        //    xpos,     ypos + h,  tex_coords[0].x, tex_coords[0].y, /*0.0f, 0.0f,*/
        //    xpos,     ypos,      tex_coords[1].x, tex_coords[1].y, /*0.0f, 1.0f,*/
        //    xpos + w, ypos,      tex_coords[2].x, tex_coords[2].y, /*1.0f, 1.0f,*/

        //    xpos,     ypos + h,  tex_coords[0].x, tex_coords[0].y, /*0.0f, 0.0f,*/
        //    xpos + w, ypos,      tex_coords[2].x, tex_coords[2].y, /*1.0f, 1.0f,*/
        //    xpos + w, ypos + h,  tex_coords[3].x, tex_coords[3].y /*1.0f, 0.0f */         
        //};

        std::array<float, 6*4> verts {
            xpos,     ypos - h,  tex_coords[0].x, tex_coords[0].y, /*0.0f, 0.0f,*/
            xpos,     ypos,      tex_coords[1].x, tex_coords[1].y, /*0.0f, 1.0f,*/
            xpos + w, ypos,      tex_coords[2].x, tex_coords[2].y, /*1.0f, 1.0f,*/

            xpos,     ypos - h,  tex_coords[0].x, tex_coords[0].y, /*0.0f, 0.0f,*/
            xpos + w, ypos,      tex_coords[2].x, tex_coords[2].y, /*1.0f, 1.0f,*/
            xpos + w, ypos - h,  tex_coords[3].x, tex_coords[3].y /*1.0f, 0.0f */         
        };

        GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, text_vbo));
        GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts.data()));
        
        GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 6));

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
    if (_triangles_vertices.empty()) return;

    int count = _triangles_vertices.size() / 3; // overall, this many triangles
    std::size_t offset = 0; // offset inside _triangles

    GL_CALL(glBindVertexArray(triangle_batch_vao));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, triangle_batch_vbo));
    _triangle_shader->bind();
    _triangle_shader->set_mat4("u_mvp", _projection);

    while (count > 0) {
        int c = 0; // count of triangles for each batch
        if (count >= MAX_TRIANGLE_COUNT) {
            c = MAX_TRIANGLE_COUNT;
        }
        else { // smaller remaining batch
            c = count;
        }
        GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, 0, 3*c*sizeof(Triangle_Vertex), _triangles_vertices.data()+offset));
        GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 3*c));
        offset += 3*c;
        count -= c;
    }

    _triangles_vertices.clear();
}

void Graphics::render_rects()
{
    if (_rects_vertices.empty()) return;

    int count = _rects_vertices.size() / 4; // overall, this many rects
    std::size_t offset = 0; // offset inside _rects

    GL_CALL(glBindVertexArray(rect_batch_vao));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, rect_batch_vbo));
    // just reuse tri shader
    _triangle_shader->bind();
    _triangle_shader->set_mat4("u_mvp", _projection);
    
    while (count > 0) {
        int c = 0; // count of rects for each batch
        if (count >= MAX_RECT_COUNT) {
            c = MAX_RECT_COUNT;
        }
        else { // smaller remaining batch
            c = count;
        }

        GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, 0, 4*c*sizeof(Rect_Vertex), _rects_vertices.data()+offset));
        GL_CALL(glDrawElements(GL_TRIANGLES, c*6, GL_UNSIGNED_INT, nullptr));
        offset += 4*c;
        count -= c;
    }

    _rects_vertices.clear();
}

void Graphics::render_circles()
{
    if (_circles_vertices.empty()) return;

    int count = _circles_vertices.size() / 4; // overall, this many rects
    std::size_t offset = 0; // offset inside _circles

    GL_CALL(glBindVertexArray(circle_batch_vao));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, circle_batch_vbo));

    _circle_shader->bind();
    _circle_shader->set_mat4("u_mvp", _projection);
    
    while (count > 0) {
        int c = 0; // count of rects for each batch
        if (count >= MAX_CIRCLE_COUNT) {
            c = MAX_CIRCLE_COUNT;
        }
        else { // smaller remaining batch
            c = count;
        }

        GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, 0, 4*c*sizeof(Circle_Vertex), _circles_vertices.data()+offset));
        GL_CALL(glDrawElements(GL_TRIANGLES, c*6, GL_UNSIGNED_INT, nullptr));
        offset += 4*c;
        count -= c;
    }

    _circles_vertices.clear();
}

void Graphics::render_text()
{
    if (_text_rects_vertices.empty()) return;

    int count = _text_rects_vertices.size() / 4; // overall, this many rects
    std::size_t offset = 0; // offset inside text _rects

    GL_CALL(glBindVertexArray(text_vao));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, text_vbo));
    // just reuse tri shader
    _text_shader->bind();
    _text_shader->set_mat4("u_mvp", _projection);
    _text_atlas->bind();
    
    while (count > 0) {
        int c = 0; // count of rects for each batch
        if (count >= MAX_RECT_COUNT) {
            c = MAX_RECT_COUNT;
        }
        else { // smaller remaining batch
            c = count;
        }

        GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, 0, 4*c*sizeof(Temp_Vertex), _text_rects_vertices.data()+offset));
        GL_CALL(glDrawElements(GL_TRIANGLES, c*6, GL_UNSIGNED_INT, nullptr));
        offset += 4*c;
        count -= c;
    }

    _text_rects_vertices.clear();
}
