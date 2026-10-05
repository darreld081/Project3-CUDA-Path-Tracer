#pragma once

#include <cuda_runtime.h>

#include "glm/glm.hpp"

#include <string>
#include <vector>

#define BACKGROUND_COLOR (glm::vec3(0.0f))

enum GeomType
{
    SPHERE,
    CUBE,
    MESH  // a triangle mesh loaded from a glTF file
};

struct Ray
{
    glm::vec3 origin;
    glm::vec3 direction;
};

struct Geom
{
    enum GeomType type;
    int materialid;
    glm::vec3 translation;
    glm::vec3 rotation;
    glm::vec3 scale;
    glm::mat4 transform;
    glm::mat4 inverseTransform;
    glm::mat4 invTranspose;

    // MESH only: this mesh owns triangles [triStart, triStart + triCount) of the
    // scene-wide triangle array, plus an object-space bounding box around them.
    int triStart;
    int triCount;
    glm::vec3 bboxMin;
    glm::vec3 bboxMax;
    int bvhRoot;  // index of this mesh's root node in the scene-wide BVH node array
};

// One box of a mesh's BVH tree (see bvh.h). Interior nodes have two children;
// leaf nodes (left == -1) own triangles [triStart, triStart + triCount).
struct BVHNode
{
    glm::vec3 bboxMin;
    glm::vec3 bboxMax;
    int left;
    int right;
    int triStart;
    int triCount;
};

// One triangle of a mesh, stored in the mesh's own (object) space.
struct Triangle
{
    glm::vec3 v0, v1, v2;  // corner positions
    glm::vec3 n0, n1, n2;  // normal at each corner (used to smooth-shade)
};

struct Material
{
    glm::vec3 color;
    struct
    {
        float exponent;
        glm::vec3 color;
    } specular;
    float hasReflective;
    float hasRefractive;
    float indexOfRefraction;
    float emittance;
};

struct Camera
{
    glm::ivec2 resolution;
    glm::vec3 position;
    glm::vec3 lookAt;
    glm::vec3 view;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec2 fov;
    glm::vec2 pixelLength;
    float radius;
    float focalDist;
};

struct RenderState
{
    Camera camera;
    unsigned int iterations;
    int traceDepth;
    std::vector<glm::vec3> image;
    std::string imageName;
};

struct PathSegment
{
    Ray ray;
    glm::vec3 color;
    int pixelIndex;
    int remainingBounces;
};

// Use with a corresponding PathSegment to do:
// 1) color contribution computation
// 2) BSDF evaluation: generate a new ray
struct ShadeableIntersection
{
  float t;
  glm::vec3 surfaceNormal;
  int materialId;
};
