
#include "locations.glsl"


struct transform
{
    mat4 world;
    mat4 wvp;
};
layout (std140, binding = CAMERA_UBO) uniform CameraDataUBO
{
    mat4 view;
    mat4 projection;
    mat4 vp;
};
layout (std140, binding = TRANSFORM_UBO) uniform TransformUBO
{
    transform mesh;
};

//debug lights
struct DirectionalLight
{
    vec3 direction;
    int dummy;
    vec3 color;
    float intensity;
};
struct PointLight
{
    vec3 position;
    int dummy;
    vec3 color;
    float intensity;
};

layout (std140, binding = DIRECTIONAL_LIGHT_UBO) uniform DirectionalLightUBO
{
    DirectionalLight directional_light;
};

layout (std140, binding = POINT_LIGHT_UBO) uniform PointLightUBO
{
    PointLight point_light;
};

#ifdef INDIGO
// very volatile this here...
IMGUI_REFLECT(DirectionalLight, direction, color, intensity)
IMGUI_REFLECT(PointLight, position, color, intensity)
#endif