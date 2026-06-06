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

// imgui
#include "imgui-master/imgui.h"
#include <imgui-master/imgui_internal.h>
#include <imgui-master/misc/cpp/imgui_stdlib.h>

#include "ImGuizmo-master/ImGuizmo.h"

#include "ImReflect-main/single_header/ImReflect.hpp"
#include "ImReflect-main/glm.hpp"

// glm https://github.com/g-truc/glm (29.10.2025) 
#define GLM_FORCE_LEFT_HANDED
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <glm/gtx/euler_angles.hpp>

// stb_image :)
// defines in precomp_impls.cpp
#include "stb_image/stb_image.h"
#include "stb_image/stb_image_write.h"

// tiny obj
// defines in precomp_impls.cpp
#include "tinyobjloader-release/tiny_obj_loader.h"

// utils
#include "common.h"
#include "log.h"
#include "timer.h"
#include "file_handler.h"

// general non-engine
#include "transform.h"

// graphics
#include "graphics/shader.h"
#include "graphics/subsystem.h"
#include "graphics/renderer.h"

// assets
#include "graphics/assets/components.h"
#include "graphics/assets/mesh.h"
#include "graphics/assets/model.h"
#include "graphics/assets/editor_camera_controller.h"

// backend specific, this will be quite useless...
#ifdef BACKEND_OPENGL

#include "device/gl/graphics_gl.h"
#include "device/gl/uniforms_gl.h"

#endif

//render subsystems -> game code actually
#include "graphics/subsystem/skydome_trace.h"
#include "graphics/subsystem/model_renderer.h"
#include "graphics/subsystem/pixel_sorter.h"

// engine
#include "input.h"
#include "graphics/graphics.h"
#include "module.h"
#include "engine.h"




// game code ???
#include "IndigoPixelSort.h"