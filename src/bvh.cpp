#include "bvh.h"

#include <algorithm>
#include <cfloat>

static const int MAX_TRIS_PER_LEAF = 4;

static glm::vec3 centroidOf(const Triangle& tri) {
    return (tri.v0 + tri.v1 + tri.v2) / 3.0f;
}

// Builds the node for triangles [start, start + count) and returns its index in nodes
static int buildNode(std::vector<Triangle>& triangles, int start, int count, std::vector<BVHNode>& nodes) {
    glm::vec3 boxMin(FLT_MAX), boxMax(-FLT_MAX);
    glm::vec3 centerMin(FLT_MAX), centerMax(-FLT_MAX);
    for (int i = start; i < start + count; i++) {
        const Triangle& tri = triangles[i];
        boxMin = glm::min(boxMin, glm::min(tri.v0, glm::min(tri.v1, tri.v2)));
        boxMax = glm::max(boxMax, glm::max(tri.v0, glm::max(tri.v1, tri.v2)));
        centerMin = glm::min(centerMin, centroidOf(tri));
        centerMax = glm::max(centerMax, centroidOf(tri));
    }

    // reserve the slot first, children get pushed after it
    int index = (int)nodes.size();
    nodes.push_back(BVHNode());
    nodes[index].bboxMin = boxMin;
    nodes[index].bboxMax = boxMax;

    if (count <= MAX_TRIS_PER_LEAF) {
        nodes[index].left = -1;
        nodes[index].right = -1;
        nodes[index].triStart = start;
        nodes[index].triCount = count;
        return index;
    }

    // split on the axis where the centroids are the most spread out
    glm::vec3 spread = centerMax - centerMin;
    int axis = 0;
    if (spread.y > spread.x) axis = 1;
    if (spread.z > spread[axis]) axis = 2;

    int mid = start + count / 2;
    std::nth_element(triangles.begin() + start, triangles.begin() + mid, triangles.begin() + start + count,
        [axis](const Triangle& a, const Triangle& b) {
            return centroidOf(a)[axis] < centroidOf(b)[axis];
        });

    // nodes can get reallocated by the recursive calls, so index into it afterwards
    int leftChild = buildNode(triangles, start, mid - start, nodes);
    int rightChild = buildNode(triangles, mid, start + count - mid, nodes);
    nodes[index].left = leftChild;
    nodes[index].right = rightChild;
    nodes[index].triStart = 0;
    nodes[index].triCount = 0;
    return index;
}

int buildBVH(std::vector<Triangle>& triangles, int triStart, int triCount, std::vector<BVHNode>& nodes) {
    return buildNode(triangles, triStart, triCount, nodes);
}
