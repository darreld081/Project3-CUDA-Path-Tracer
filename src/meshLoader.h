#pragma once

#include "sceneStructs.h"

#include <string>
#include <vector>

// Loads a glTF file
bool loadGltfMesh(const std::string& path, std::vector<Triangle>& outTriangles, glm::vec3& bboxMin, glm::vec3& bboxMax);
