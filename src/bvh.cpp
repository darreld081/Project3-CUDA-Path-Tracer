#include "bvh.h"

#include <algorithm>
#include <cfloat>

// A node with this many triangles or fewer stops splitting and becomes a leaf.
static const int MAX_TRIANGLES_PER_LEAF = 4;

static glm::vec3 triangleCentroid(const Triangle& t)
{
    return (t.v0 + t.v1 + t.v2) / 3.0f;
}

// Recursively builds the node for triangles [start, start + count) and returns its index.
static int buildNode(std::vector<Triangle>& triangles, int start, int count,
                     std::vector<BVHNode>& nodes)
{
    // Box around all triangles in this range, and box around just their centroids.
    glm::vec3 boxMin(FLT_MAX), boxMax(-FLT_MAX);
    glm::vec3 centroidMin(FLT_MAX), centroidMax(-FLT_MAX);
    for (int i = start; i < start + count; ++i)
    {
        const Triangle& t = triangles[i];
        boxMin = glm::min(boxMin, glm::min(t.v0, glm::min(t.v1, t.v2)));
        boxMax = glm::max(boxMax, glm::max(t.v0, glm::max(t.v1, t.v2)));
        centroidMin = glm::min(centroidMin, triangleCentroid(t));
        centroidMax = glm::max(centroidMax, triangleCentroid(t));
    }

    // Reserve this node's slot now (children get added after it).
    int nodeIndex = (int)nodes.size();
    nodes.push_back(BVHNode());
    nodes[nodeIndex].bboxMin = boxMin;
    nodes[nodeIndex].bboxMax = boxMax;

    // Small enough: make a leaf that owns these triangles directly.
    if (count <= MAX_TRIANGLES_PER_LEAF)
    {
        nodes[nodeIndex].left = -1;
        nodes[nodeIndex].right = -1;
        nodes[nodeIndex].triStart = start;
        nodes[nodeIndex].triCount = count;
        return nodeIndex;
    }

    // Otherwise split along the axis where the triangle centers are most spread out.
    glm::vec3 extent = centroidMax - centroidMin;
    int axis = 0;
    if (extent.y > extent.x) axis = 1;
    if (extent.z > extent[axis]) axis = 2;

    // Put the lower half of the triangles (by center position) first, the upper half second.
    int mid = start + count / 2;
    std::nth_element(triangles.begin() + start, triangles.begin() + mid, triangles.begin() + start + count,
        [axis](const Triangle& a, const Triangle& b) {
            return triangleCentroid(a)[axis] < triangleCentroid(b)[axis];
        });

    // NOTE: don't hold a reference into `nodes` across these calls: push_back may move it.
    int left = buildNode(triangles, start, mid - start, nodes);
    int right = buildNode(triangles, mid, start + count - mid, nodes);
    nodes[nodeIndex].left = left;
    nodes[nodeIndex].right = right;
    nodes[nodeIndex].triStart = 0;
    nodes[nodeIndex].triCount = 0;
    return nodeIndex;
}

int buildBVH(std::vector<Triangle>& triangles, int triStart, int triCount,
             std::vector<BVHNode>& nodes)
{
    return buildNode(triangles, triStart, triCount, nodes);
}
