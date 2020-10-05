#pragma once
#include "usami/memory/arena.h"
#include "usami/memory/buffer.h"
#include "usami/math/math.h"
#include "usami/texture.h"
#include <string>
#include <span>

namespace usami
{
    struct TriangleVertexDesc
    {
        std::array<Array3f, 3> vertices;
    };

    struct TriangleDesc
    {
        std::array<Array3f, 3> vertices;
        std::array<Array3f, 3> normals;
        std::array<Array2f, 3> tex_coords;

        bool has_normal;
        bool has_tex_coord;
    };

    struct SceneDataBuffer
    {
        std::unique_ptr<std::byte[]> data;
        size_t size;   // in bytes
        size_t stride; // in bytes
    };

    template <typename T, size_t N>
    struct SceneDataBufferView
    {
        SceneDataBuffer* buffer;
        size_t offset; // in bytes

        const std::byte* PtrAt(size_t index) const noexcept
        {
            return buffer->data.get() + index * buffer->stride + offset;
        }

        std::span<T, N> CreateView(const std::byte* ptr) const noexcept
        {
            return std::span<T, N>{reinterpret_cast<const T*>(ptr), N};
        }
    };

    struct SceneMaterial
    {
        std::string name;

        Array3f emissive_factor;
        Texture2D<Vec3f>* emissive_texture;

        Array3f base_color_factor;
        Texture2D<Vec3f>* base_color_texture;

        float matallic_factor;
        float roughness_factor;
    };

    struct SceneMesh
    {
        std::string name;

        // number of triangles
        size_t num_face;

        SceneDataBufferView<uint32_t, 1> indices;

        SceneDataBufferView<float, 3> vertices;
        SceneDataBufferView<float, 3> normals;
        SceneDataBufferView<float, 2> tex_coords;

        SceneMaterial* material;

        TriangleVertexDesc GetTriangleVertices(size_t iface) const
        {
            const std::byte* p_index = indices.PtrAt(3 * iface);
            // USAMI_ASSERT((p_index - indices.offset + 3 * indices.buffer->stride) -
            //                  indices.buffer->data.get() <
            //              indices.buffer->size);

            Array3i index_vals;
            for (int i = 0; i < 3; ++i)
            {
                index_vals[i] = *reinterpret_cast<const uint32_t*>(p_index);
                p_index += indices.buffer->stride;
            }

            TriangleVertexDesc result;

            // copy vertex positions
            for (int i = 0; i < 3; ++i)
            {
                memcpy(&result.vertices[i], vertices.PtrAt(index_vals[i]),
                       sizeof(result.vertices[i]));
            }

            return result;
        }

        TriangleDesc GetTriangle(size_t iface) const
        {
            const std::byte* p_index = indices.PtrAt(iface);
            USAMI_ASSERT((p_index - indices.offset + 3 * indices.buffer->stride) -
                             indices.buffer->data.get() <
                         indices.buffer->size);

            Array3i index_vals;
            for (int i = 0; i < 3; ++i)
            {
                index_vals[i] = *reinterpret_cast<const uint32_t*>(p_index);
                p_index += indices.buffer->stride;
            }

            TriangleDesc result;

            // copy vertex positions
            for (int i = 0; i < 3; ++i)
            {
                memcpy(&result.vertices[i], vertices.PtrAt(index_vals[i]),
                       sizeof(result.vertices[i]));
            }

            // copy normals if any
            if (normals.buffer != nullptr)
            {
                result.has_normal = true;
                for (int i = 0; i < 3; ++i)
                {
                    memcpy(&result.normals[i], normals.PtrAt(index_vals[i]),
                           sizeof(result.normals[i]));
                }
            }
            else
            {
                result.has_normal = false;
            }

            // copy texture coordinate if any
            if (tex_coords.buffer != nullptr)
            {
                result.has_tex_coord = true;
                for (int i = 0; i < 3; ++i)
                {
                    memcpy(&result.tex_coords[i], tex_coords.PtrAt(index_vals[i]),
                           sizeof(result.tex_coords[i]));
                }
            }
            else
            {
                result.has_tex_coord = false;
            }

            return result;
        }
    };

    struct SceneNode
    {
        std::string name;

        SceneMesh* mesh;

        Matrix4 transform; // TODO: add Translate-Rotate-Scale interface

        std::vector<SceneNode*> children;
    };

    struct SceneModel
    {
        std::vector<unique_ptr<Texture2D<Vec3f>>> textures;

        std::vector<unique_ptr<SceneDataBuffer>> buffers;
        std::vector<unique_ptr<SceneMaterial>> materials;
        std::vector<unique_ptr<SceneMesh>> meshes;
        std::vector<unique_ptr<SceneNode>> nodes;

        std::string name;
        std::vector<SceneNode*> roots;
    };

    shared_ptr<SceneModel> ParseModel(const std::string& filename, bool binary = false);

} // namespace usami
