#version 460
layout (location = 0) in vec2 _pos;

uniform mat4 u_mvp;

void main()
{
    gl_Position = u_mvp*vec4(_pos.xy, 0.0f, 1.0f);
}
