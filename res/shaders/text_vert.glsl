#version 460

layout (location = 0) in vec2 _pos;
layout (location = 1) in vec2 _tex;
layout (location = 2) in vec4 _text_color;

uniform mat4 u_mvp;
out vec2 tex;
out vec4 text_color;

void main()
{
    gl_Position = u_mvp*vec4(_pos.xy, 0.0f, 1.0f);
    tex = _tex;
    text_color = _text_color;
}
