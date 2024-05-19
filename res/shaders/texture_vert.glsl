#version 460

layout (location = 0) in vec2 _pos;
layout (location = 1) in vec2 _tex_coord;

uniform mat4 u_mvp;

out vec2 tex_coord;

void main()
{
    tex_coord = _tex_coord;
    gl_Position = u_mvp*vec4(_pos.xy, 0.0f, 1.0f);
}
