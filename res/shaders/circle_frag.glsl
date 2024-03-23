#version 460

in vec4 color;
out vec4 final_color;

uniform float u_radius;
uniform vec2 u_center;

void main()
{
    vec2 p = gl_FragCoord.xy;
    p -= u_center; // translate to origin
    if (p.x*p.x+p.y*p.y <= u_radius*u_radius) {
        final_color = color;
    }
    else {
        discard;
    }
}
