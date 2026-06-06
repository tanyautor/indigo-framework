#version 450 core

#include "uniforms.glsl"

in vec2 v_texture;
in vec3 v_position;
in vec3 v_color;
in vec3 v_normal;

uniform sampler2D diffuse;
uniform sampler2D normal;

out vec4 FragColor;

vec4 ambient_light = vec4(1,1,1, 0.1);

vec3 light_attenuation(vec3 norm)
{
    vec3 light;

    // directional light
    float dir_att = max(dot(norm, normalize(directional_light.direction)), 0.0);
    light += vec3(dir_att * directional_light.color) * directional_light.intensity;
    
    // point light
    vec3 point_light_direction = normalize(point_light.position - v_position);
    float point_att = max(dot(norm, point_light_direction), 0.0);
    light += vec3(point_att * point_light.color) * point_light.intensity;
    
    return light;
}

void main()
{
    vec3 texel = texture(diffuse, v_texture).xyz;
    vec3 ntexel = texture(normal, v_texture).xyz;

    vec3 surface_lighting = light_attenuation(ntexel);
    vec3 vertex_lighting = light_attenuation(v_normal);
    
    vec3 light;
    light += ambient_light.rgb * ambient_light.a;
    light += surface_lighting.rgb;
    light += vertex_lighting.rgb;

    vec3 color = texel * light;

    FragColor = vec4(color, 1.0);
} 