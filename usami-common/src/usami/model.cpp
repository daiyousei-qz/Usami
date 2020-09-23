#include "usami/model.h"
#include "usami/texture/image.h"
#include "tiny_obj_loader.h"
#include <filesystem>

using namespace std;
using namespace std::filesystem;

namespace usami
{
    static void ParseMaterial(MaterialDesc& desc, const tinyobj::material_t& mat, const path& dir,
                              unordered_map<string, shared_ptr<Texture2D<Vec3f>>>& tex_lookup)
    {
        const auto try_load_texture = [&](const string& name) -> shared_ptr<Texture2D<Vec3f>> {
            if (name.empty())
            {
                return nullptr;
            }

            if (auto iter = tex_lookup.find(name); iter != tex_lookup.end())
            {
                return iter->second;
            }
            else
            {
                auto tex_path = dir / name;
                auto tex      = make_shared<ImageTexture>(tex_path.string());

                tex_lookup[name] = tex;
                return tex;
            }
        };

        desc.name     = mat.name;
        desc.ka       = Vec3f{mat.ambient};
        desc.kd       = Vec3f{mat.diffuse};
        desc.ks       = Vec3f{mat.specular};
        desc.tr       = Vec3f{mat.transmittance};
        desc.emmision = Vec3f{mat.emission};

        desc.tex_ambient = try_load_texture(mat.ambient_texname);
        desc.tex_ambient = try_load_texture(mat.diffuse_texname);
        desc.tex_ambient = try_load_texture(mat.specular_texname);
    }

    shared_ptr<MeshDesc> LoadModel(const string& filename)
    {
        path file_path = filename;
        path dir_path  = file_path.parent_path();

        tinyobj::ObjReader obj_reader{};
        tinyobj::ObjReaderConfig config{};
        config.mtl_search_path = dir_path.string();

        bool success = obj_reader.ParseFromFile(filename);
        USAMI_REQUIRE(success);

        auto result  = make_shared<MeshDesc>();
        result->name = filename;

        result->vertices   = obj_reader.GetAttrib().vertices;
        result->normals    = obj_reader.GetAttrib().normals;
        result->tex_coords = obj_reader.GetAttrib().texcoords;

        for (const auto& mat : obj_reader.GetMaterials())
        {
            ParseMaterial(result->materials.emplace_back(), mat, dir_path, result->texture_lookup);
        }

        std::vector<GeometryDesc*> geom_map{result->materials.size() + 1};
        for (const auto& shape : obj_reader.GetShapes())
        {
            std::fill(geom_map.begin(), geom_map.end(), nullptr);

            auto vertex_iter = shape.mesh.indices.begin();
            for (int iface = 0; iface < shape.mesh.num_face_vertices.size(); ++iface)
            {
                int face_vertex_count = shape.mesh.num_face_vertices[iface];
                int face_material_id  = shape.mesh.material_ids[iface];

                auto v0 = *vertex_iter;
                ++vertex_iter;

                auto v1 = *vertex_iter;
                ++vertex_iter;

                auto& geom = geom_map[face_material_id + 1];
                if (geom == nullptr)
                {
                    geom = &result->geometries.emplace_back();

                    geom->name        = shape.name;
                    geom->material_id = face_material_id;
                }

                for (int j = 2; j < face_vertex_count; ++j)
                {
                    auto v2 = *vertex_iter;
                    ++vertex_iter;

                    // copy vertex index
                    geom->vertex_indices.push_back(v0.vertex_index);
                    geom->vertex_indices.push_back(v1.vertex_index);
                    geom->vertex_indices.push_back(v2.vertex_index);

                    // copy normal index if any
                    if (!result->normals.empty() && v0.normal_index != -1)
                    {
                        geom->normal_indices.push_back(v0.normal_index);
                        geom->normal_indices.push_back(v1.normal_index);
                        geom->normal_indices.push_back(v2.normal_index);
                    }

                    // copy tex_coord index if any
                    if (!result->tex_coords.empty() && v0.texcoord_index != -1)
                    {
                        geom->tex_coords_indices.push_back(v0.texcoord_index);
                        geom->tex_coords_indices.push_back(v1.texcoord_index);
                        geom->tex_coords_indices.push_back(v2.texcoord_index);
                    }
                }
            }
        }

        return result;
    }
} // namespace usami
