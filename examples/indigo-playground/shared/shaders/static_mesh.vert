
#version 450 core

#include "uniforms.glsl"

layout (location = MESH_POSITION)   in vec3 a_position;
layout (location = MESH_NORMAL)     in vec3 a_normal;
layout (location = MESH_TEXTURE0)   in vec2 a_texcoords;
layout (location = MESH_TEXTURE1)   in vec2 a_texcoords1;
layout (location = MESH_COLOR)      in vec3 a_color;
layout (location = MESH_TANGENT)    in vec3 a_tangent;

out vec2 v_texture;
out vec3 v_position;
out vec3 v_color;
out vec3 v_normal;

void main()
{
    v_texture = a_texcoords;
    v_position = vec3(mesh.world * vec4(a_position, 0.0));
    v_color = a_color;
    v_normal = normalize((mesh.world * vec4(a_normal, 0.0)).xyz);

    gl_Position = mesh.wvp * vec4(a_position, 1.0);
}
 