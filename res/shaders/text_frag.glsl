#version 460

in vec2 tex;
in vec4 text_color;

out vec4 final_color;

uniform sampler2D u_text;

void main()
{
    vec4 sampled = vec4(1.0f, 1.0f, 1.0f, texture(u_text, tex).r);
    final_color = text_color * sampled;
}
