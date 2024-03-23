#version 460
layout (location = 0) in vec2 _pos;
layout (location = 1) in vec4 _color;

uniform mat4 u_mvp;

out vec4 color;

void main()
{
    gl_Position = u_mvp*vec4(_pos.xy, 0.0f, 1.0f);
	color = _color;
}
