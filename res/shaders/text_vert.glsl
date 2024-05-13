#version 460

layout (location = 0) in vec2 _pos;
layout (location = 1) in vec2 _tex;

uniform mat4 u_mvp;
out vec2 tex;

void main()
{
    gl_Position = u_mvp*vec4(_pos.xy, 0.0f, 1.0f);
    tex = _tex;
}
