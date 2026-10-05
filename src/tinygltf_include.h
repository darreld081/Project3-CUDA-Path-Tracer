#pragma once

// include tinygltf only through this header so every file uses the same defines
// (reuse our json.hpp, skip image loading since we don't use textures)
#include <json.hpp>

#define TINYGLTF_NO_INCLUDE_JSON
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_EXTERNAL_IMAGE
#include "tiny_gltf.h"
