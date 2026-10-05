#pragma once

#include "sceneStructs.h"
#include <vector>

class Scene
{
private:
    void loadFromJSON(const std::string& jsonName);
public:
    Scene(std::string filename);

    std::vector<Geom> geoms;
    std::vector<BVHNode> bvhNodes;    // BVH boxes of every mesh, back to back
    std::vector<Triangle> triangles;  // triangles of every mesh, back to back
    std::vector<Material> materials;
    RenderState state;
};
