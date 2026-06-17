#pragma once

// std
#include <stdio.h>
#include <stdlib.h>
#include <cassert>
#include <iostream>
#include <string>
#include <sstream>
#include <chrono>
#include <optional>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <regex>


// fmt https://github.com/fmtlib/fmt (29.10.2025)
#define FMT_HEADER_ONLY
#include "fmt/include/fmt/core.h"

// magic_enum https://github.com/Neargye/magic_enum/tree/master (29.10.2025)
#include "magic_enum/magic_enum.hpp"

// glm https://github.com/g-truc/glm (29.10.2025) 
//#define GLM_FORCE_LEFT_HANDED
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/euler_angles.hpp"
using namespace glm;

// imgui
#ifdef INDIGO_EDITOR

#include "imgui/imgui.h"
#include "imgui_impl.h"
#include "imgui/imgui_internal.h"
#include "imgui/misc/cpp/imgui_stdlib.h"

#include "ImGuizmo/ImGuizmo.h"

#include "ImReflect/single_header/ImReflect.hpp"
#include "ImReflect/glm.hpp"

#endif

// stb_image :)
// defines in precomp_impls.cpp
#include "stb_image/stb_image.h"
#include "stb_image/stb_image_write.h"

// tiny obj
// defines in precomp_impls.cpp
#include "tinyobjloader/tiny_obj_loader.h"

// tiny gltf
#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#define TINYGLTF_NO_INCLUDE_STB_IMAGE_WRITE
#include "tinygltf/tiny_gltf.h"

// utils
#include "common.h"
#include "utils/types.h"
#include "utils/log.h"
using namespace indigo_log;
#include "utils/timer.h"
#include "utils/file_handler.h"

#include "common/resource.h"
#include "common/editor_interfaces.h"
#include "common/module.h"

// graphics
#include "graphics/shader.h"
#include "graphics/subsystem.h"
#include "graphics/renderer.h"

// backend specific, this will be quite useless...
#ifdef BACKEND_OPENGL

#include "graphics/gl/graphics_gl.h"
#include "graphics/gl/uniforms_gl.h"

#endif

// common assets components
#include "common/transform.h"
#include "common/camera.h"
#include "common/light.h"
#include "common/model.h"
#include "common/texture.h"
#include "common/material.h"
#include "common/mesh.h"

// Modules
#include "modules/editor.h"
#include "modules/editor_camera_controller.h"
#include "modules/resource_manager.h"

//render subsystems -> game code actually
#include "graphics/subsystem/skydome_trace.h"
#include "graphics/subsystem/mesh_renderer.h"

// engine
#include "input.h"
#include "graphics/graphics.h"
#include "engine.h"

