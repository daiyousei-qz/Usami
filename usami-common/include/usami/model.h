#pragma once
#include "usami/texture.h"
#include <vector>
#include <string>
#include <unordered_map>

namespace usami
{
    struct MaterialDesc
    {
        std::string name;

        Vec3f ka = 0.f; // ambient
        Vec3f kd = 0.f; // diffuse
        Vec3f ks = 0.f; // specular
        Vec3f tr = 0.f; // transimssion

        Vec3f emmision = 0.f;

        shared_ptr<Texture2D<Vec3f>> tex_ambient  = nullptr;
        shared_ptr<Texture2D<Vec3f>> tex_diffuse  = nullptr;
        shared_ptr<Texture2D<Vec3f>> tex_specular = nullptr;

        // pbr
    };

    struct GeometryDesc
    {
        std::string name;

        int material_id;

        // NOTE actually index offset should be 3*indices[i]
        std::vector<int> vertex_indices;
        // NOTE actually index offset should be 3*indices[i]
        std::vector<int> normal_indices;
        // NOTE actually index offset should be 2*indices[i]
        std::vector<int> tex_coords_indices;
    };

    struct MeshDesc
    {
        // usually path of .obj file
        std::string name;

        std::vector<float> vertices;
        std::vector<float> normals;
        std::vector<float> tex_coords;

        std::unordered_map<std::string, shared_ptr<Texture2D<Vec3f>>> texture_lookup;

        std::vector<MaterialDesc> materials;
        std::vector<GeometryDesc> geometries;
    };

    shared_ptr<MeshDesc> LoadModel(const std::string& filename);
} // namespace usami
