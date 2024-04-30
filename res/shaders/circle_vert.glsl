#version 460
layout (location = 0) in vec2 _pos;
layout (location = 1) in vec2 _center_world;
layout (location = 2) in vec4 _color;
layout (location = 3) in float _radius;

uniform mat4 u_mvp;

out float radius;
out vec2 center_world;
out vec4 color;

void main()
{
    gl_Position = u_mvp*vec4(_pos.xy, 0.0f, 1.0f);
	color = _color;
	center_world = _center_world;
	radius = _radius;
}
