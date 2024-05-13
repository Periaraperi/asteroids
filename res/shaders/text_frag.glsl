#version 460

in vec2 tex;
out vec4 final_color;
uniform sampler2D u_text;
uniform vec3 u_color;

void main()
{
    vec4 sampled = vec4(1.0f, 1.0f, 1.0f, texture(u_text, tex).r);
    final_color = vec4(u_color, 1.0f) * sampled;
}
