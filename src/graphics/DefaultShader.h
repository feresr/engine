#pragma once
#include "Shader.h"

static const Engine::ShaderData shader_data = {
    // vertex shader
    "#version 330\n"
    "uniform mat4 u_matrix;\n"
    "layout(location=0) in vec2 a_position;\n"
    "layout(location=1) in vec2 a_tex;\n"
    "layout(location=2) in vec4 a_color;\n"
    "layout(location=3) in vec4 a_type;\n"
    "out vec2 v_tex;\n"
    "out vec4 v_col;\n"
    "out vec4 v_type;\n"
    "void main(void)\n"
    "{\n"
    "	gl_Position = u_matrix * vec4(a_position.xy, 0, 1);\n"
    "	v_tex = a_tex;\n"
    "	v_col = a_color;\n"
    "	v_type = a_type;\n"
    "}",

    // fragment shader
    "#version 330\n"
    "uniform sampler2D u_texture;\n"
    "in vec2 v_tex;\n"
    "in vec4 v_col;\n"
    "in vec4 v_type;\n"
    "out vec4 o_color;\n"
    "void main(void)\n"
    "{\n"
    "	vec4 color = texture(u_texture, v_tex);\n"
    "	o_color = \n"
    // multiply (multiply * texture color * passed in color)
    "		v_type.x * color * v_col + \n"
    // wash (wash * alpha * passed in color)
    "		v_type.y * color.a * v_col + \n"
    // fill (passed in color)
    "		v_type.z * v_col;\n"
    "}"};