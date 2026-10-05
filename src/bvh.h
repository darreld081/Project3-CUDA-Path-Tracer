#pragma once

#include "sceneStructs.h"

#include <vector>

// Builds a BVH over one mesh's triangles and returns the root node index.
// Reorders triangles[triStart, triStart + triCount) so each leaf owns a contiguous run.
int buildBVH(std::vector<Triangle>& triangles, int triStart, int triCount, std::vector<BVHNode>& nodes);
