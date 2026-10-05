#pragma once

// Include tinygltf through this header everywhere, so every file sees the same
// settings. We reuse the project's own json.hpp and skip texture loading, so
// tinygltf doesn't bring a second copy of nlohmann json or stb_image.
#include <json.hpp>

#define TINYGLTF_NO_INCLUDE_JSON
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_EXTERNAL_IMAGE
#include "tiny_gltf.h"
