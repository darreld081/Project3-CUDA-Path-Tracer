#include "meshLoader.h"
#include "tinygltf_include.h"
#include <cfloat>
#include <iostream>


static const unsigned char* accessorItem(const tinygltf::Model& model, const tinygltf::Accessor& accessor,size_t i) {
    const tinygltf::BufferView& view = model.bufferViews[accessor.bufferView];
    const tinygltf::Buffer& buffer = model.buffers[view.buffer];
    size_t stride = accessor.ByteStride(view);  // tinygltf works out the packed size for us
    return buffer.data.data() + view.byteOffset + accessor.byteOffset + i * stride;
}

static std::vector<glm::vec3> readVec3Attribute(const tinygltf::Model& model, const tinygltf::Accessor& accessor) {
    std::vector<glm::vec3> out(accessor.count);
    for (size_t i = 0; i < accessor.count; i++)
    {
        const float* f = reinterpret_cast<const float*>(accessorItem(model, accessor, i));
        out[i] = glm::vec3(f[0], f[1], f[2]);
    }
    return out;
}

static std::vector<int> readIndices(const tinygltf::Model& model, const tinygltf::Accessor& accessor) {
    std::vector<int> out(accessor.count);
    for (size_t i = 0; i < accessor.count; i++)
    {
        const unsigned char* p = accessorItem(model, accessor, i);
        switch (accessor.componentType)
        {
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
            out[i] = *p;
            break;
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
            out[i] = *reinterpret_cast<const unsigned short*>(p);
            break;
        default:  // TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT
            out[i] = (int)*reinterpret_cast<const unsigned int*>(p);
            break;
        }
    }
    return out;
}

static void appendPrimitive(const tinygltf::Model& model,
    const tinygltf::Primitive& prim, std::vector<Triangle>& outTriangles, glm::vec3& bboxMin, glm::vec3& bboxMax)
{
    if (prim.mode != TINYGLTF_MODE_TRIANGLES) return;
    if (prim.attributes.count("POSITION") == 0) return;

    std::vector<glm::vec3> positions =
        readVec3Attribute(model, model.accessors[prim.attributes.at("POSITION")]);

    bool hasNormals = prim.attributes.count("NORMAL") > 0;
    std::vector<glm::vec3> normals;
    if (hasNormals) {
        normals = readVec3Attribute(model, model.accessors[prim.attributes.at("NORMAL")]);
    }
    std::vector<int> indices;
    if (prim.indices >= 0) {
        indices = readIndices(model, model.accessors[prim.indices]);
    }
    else  {
        for (size_t i = 0; i < positions.size(); i++) indices.push_back((int)i);
    }
    for (size_t i = 0; i + 2 < indices.size(); i += 3) {
        Triangle tri;
        tri.v0 = positions[indices[i]];
        tri.v1 = positions[indices[i + 1]];
        tri.v2 = positions[indices[i + 2]];

        if (hasNormals) {
            tri.n0 = normals[indices[i]];
            tri.n1 = normals[indices[i + 1]];
            tri.n2 = normals[indices[i + 2]];
        }
        else {
            glm::vec3 faceNormal = glm::normalize(glm::cross(tri.v1 - tri.v0, tri.v2 - tri.v0));
            tri.n0 = tri.n1 = tri.n2 = faceNormal;
        }
        bboxMin = glm::min(bboxMin, glm::min(tri.v0, glm::min(tri.v1, tri.v2)));
        bboxMax = glm::max(bboxMax, glm::max(tri.v0, glm::max(tri.v1, tri.v2)));
        outTriangles.push_back(tri);
    }
}

bool loadGltfMesh(const std::string& path,
                  std::vector<Triangle>& outTriangles,
                  glm::vec3& bboxMin,
                  glm::vec3& bboxMax) {
    tinygltf::TinyGLTF loader;
    tinygltf::Model model;
    std::string err, warn;

    bool isBinary = path.size() >= 4 && path.substr(path.size() - 4) == ".glb";
    bool ok = isBinary ? loader.LoadBinaryFromFile(&model, &err, &warn, path)
                       : loader.LoadASCIIFromFile(&model, &err, &warn, path);
    if (!warn.empty()) std::cout << "glTF warning: " << warn << std::endl;
    if (!ok) {
        std::cout << "glTF error: " << err << std::endl;
        return false;
    }

    bboxMin = glm::vec3(FLT_MAX);
    bboxMax = glm::vec3(-FLT_MAX);

    size_t before = outTriangles.size();
    for (const tinygltf::Mesh& mesh : model.meshes) {
        for (const tinygltf::Primitive& prim : mesh.primitives) {
            appendPrimitive(model, prim, outTriangles, bboxMin, bboxMax);
        }
    }

    size_t count = outTriangles.size() - before;
    std::cout << "Loaded " << path << ": " << count << " triangles" << std::endl;
    return count > 0;
}
