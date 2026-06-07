
// from: learnopengl.com (16.03.2026)
#version 450 core

#include "uniforms.glsl"

layout (location = 0) in vec2 a_pos;
layout (location = 1) in vec2 a_texcoords;

out vec2 v_pos;

void main()
{
    gl_Position = vec4(a_pos.x, a_pos.y, 0.0, 1.0); 
    v_pos = a_pos.xy;
}  