#pragma once

#include "sceneStructs.h"

#include <vector>

// Builds a BVH (bounding volume hierarchy) over one mesh's triangles.
//
// A BVH is a tree of boxes. The root box holds the whole mesh; each box is split
// into two child boxes holding half of its triangles each; the small boxes at the
// bottom (leaves) hold just a few triangles. A ray that misses a box can skip
// everything below it, so it only tests a handful of triangles instead of all.
//
// * triangles[triStart .. triStart+triCount) is REORDERED in place so that every
//   leaf owns a contiguous run of triangles.
// * New nodes are appended to `nodes`; child links are indices into that array.
// * Returns the index of the root node.
int buildBVH(std::vector<Triangle>& triangles, int triStart, int triCount,
             std::vector<BVHNode>& nodes);
