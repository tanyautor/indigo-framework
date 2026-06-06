#pragma once


#define uint uint32
#define vec2 glm::vec2
#define vec3 glm::vec3
#define ivec2 glm::ivec2
#define ivec3 glm::ivec3
#define ivec4 glm::ivec4
#define vec4 glm::vec4
#define mat4 glm::mat4
#define mat3 glm::mat3
#define uniform struct
#define layout(x, ... )

#define image2D
#define image3D
#define uimage3D
#define sampler2D
#define sampler3D

// binding locations -> basically just defines
#include "shaders/locations.glsl"

// uniform definitions
#include "shaders/uniforms.glsl"

#include "shaders/pixel_sort_locations.glsl"
#include "shaders/pixel_sort_uniforms.glsl"

#undef uint
#undef vec2 
#undef vec3 
#undef ivec2
#undef ivec3 
#undef ivec4 
#undef vec4 
#undef mat4 
#undef mat3 
#undef uniform
