#version 460

in vec2 tex_coord;
out vec4 final_color;

uniform sampler2D u_texture;

void main()
{
    final_color = texture(u_texture, tex_coord);
}
