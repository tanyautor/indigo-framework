
// from: learnopengl.com (16.03.2026)
#version 450 core

#include "uniforms.glsl"

#define PI 3.15149

out vec4 FragColor;
  
in vec2 v_pos;

uniform sampler2D skydome_texture;

vec3 sample_sky(vec3 dir)
{
    vec3 direction = normalize(dir);

	// Sample Sky
    float u = atan(direction.z, direction.x) / (2.0 * PI) + 0.5;
    float v = acos(direction.y) / PI;

    return texture(skydome_texture, vec2(u,v)).rgb;
}

void main()
{     
    vec4 direction = inverse(projection) * vec4(v_pos.xy, 1.0, 1.0);
    direction.xyz /= direction.w;
    vec3 world_dir = mat3(inverse(view)) * direction.xyz;
    FragColor = vec4(sample_sky(world_dir.xyz), 1);
}