#version 330

uniform sampler2D u_texture;
uniform vec2 lightPos;
in vec2 v_tex;

out vec4 o_color;

void main() {
    vec4 color = texture(u_texture, v_tex);
    vec2 fc = gl_FragCoord.xy;

    o_color = vec4(color.xyz * (0.96 + 0.04 * sin(v_tex.y * 600)), color.w);
}