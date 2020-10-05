
#define TINYGLTF_NO_INCLUDE_JSON
#include "nlohmann/json.hpp"

#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#include "stb_image.h"

#define TINYGLTF_NO_INCLUDE_STB_IMAGE_WRITE
#include "stb_image_write.h"

#define TINYGLTF_USE_CPP14
#define TINYGLTF_IMPLEMENTATION
#include "tiny_gltf.h"

#include "usami/math/vector.h"
#include "usami/mesh.h"
#include "usami/texture/image.h"

namespace usami
{
    template <typename TDst, typename TSrc, size_t N>
    static unique_ptr<SceneDataBuffer>
    AssignConstructBuffer(const tinygltf::Accessor& gltf_accessor,
                          const tinygltf::BufferView& gltf_bufview,
                          const tinygltf::Buffer& gltf_buffer)
    {
        size_t src_byte_stride =
            gltf_bufview.byteStride != 0 ? gltf_bufview.byteStride : sizeof(TSrc);

        // allocate buffer
        unique_ptr<SceneDataBuffer> buffer = make_unique<SceneDataBuffer>();

        // fill metadata
        size_t cell_size  = N * sizeof(TDst);
        size_t cell_count = gltf_accessor.count;
        buffer->data      = make_unique<std::byte[]>(cell_count * cell_size);
        buffer->size      = cell_count * cell_size;
        buffer->stride    = cell_size;

        // copy data
        const std::byte* src_buffer = reinterpret_cast<const std::byte*>(gltf_buffer.data.data());

        const std::byte* p_src = src_buffer + gltf_bufview.byteOffset;
        std::byte* p_dst       = buffer->data.get();

        for (int i = 0; i < cell_count; ++i)
        {
            const TSrc* p_src_typed = reinterpret_cast<const TSrc*>(p_src);
            TDst* p_dst_typed       = reinterpret_cast<TDst*>(p_dst);

            for (int j = 0; j < N; ++j)
            {
                p_dst_typed[j] = p_src_typed[j];
            }
            p_src += src_byte_stride;
            p_dst += cell_size;
        }

        return buffer;
    }

    template <typename T, size_t N>
    static unique_ptr<SceneDataBuffer> CopyConstructBuffer(const tinygltf::Accessor& gltf_accessor,
                                                           const tinygltf::BufferView& gltf_bufview,
                                                           const tinygltf::Buffer& gltf_buffer)
    {
        size_t src_byte_stride = gltf_bufview.byteStride != 0 ? gltf_bufview.byteStride : sizeof(T);

        // allocate buffer
        unique_ptr<SceneDataBuffer> buffer = make_unique<SceneDataBuffer>();

        // fill metadata
        size_t cell_size  = N * sizeof(T);
        size_t cell_count = gltf_accessor.count;
        buffer->data      = make_unique<std::byte[]>(cell_count * cell_size);
        buffer->size      = cell_count * cell_size;
        buffer->stride    = cell_size;

        // copy data
        const std::byte* src_buffer = reinterpret_cast<const std::byte*>(gltf_buffer.data.data());

        const std::byte* p_src = src_buffer + gltf_bufview.byteOffset + gltf_accessor.byteOffset;
        std::byte* p_dst       = buffer->data.get();

        if (cell_size == gltf_bufview.byteStride)
        {
            memcpy(p_dst, p_src, buffer->size);
        }
        else
        {
            for (int i = 0; i < cell_count; ++i)
            {
                memcpy(p_dst, p_src, cell_size);
                p_src += src_byte_stride;
                p_dst += cell_size;
            }
        }

        return buffer;
    }

    shared_ptr<SceneModel> ParseModel(const std::string& filename, bool binary)
    {
        tinygltf::Model gltf_model;
        tinygltf::TinyGLTF loader;
        std::string err;
        std::string warn;

        bool success;
        if (binary)
        {
            success = loader.LoadBinaryFromFile(&gltf_model, &err, &warn, filename);
        }
        else
        {
            success = loader.LoadASCIIFromFile(&gltf_model, &err, &warn, filename);
        }

        if (!success)
        {
            return nullptr;
        }

        shared_ptr<SceneModel> result = make_shared<SceneModel>();

        // load texture
        for (const auto& gltf_texture : gltf_model.textures)
        {
            // TODO: support sampler
            const auto& gltf_img = gltf_model.images.at(gltf_texture.source);
            // USAMI_REQUIRE(!gltf_img.uri.empty());
            USAMI_REQUIRE(gltf_img.bits == 8);
            USAMI_REQUIRE(gltf_img.pixel_type == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE);

            result->textures.push_back(make_unique<ImageTexture>(
                gltf_img.image.data(), gltf_img.width, gltf_img.height, gltf_img.component));
        }

        // load material
        for (const auto& gltf_material : gltf_model.materials)
        {
            auto& material = result->materials.emplace_back(make_unique<SceneMaterial>());

            material->name            = gltf_material.name;
            material->emissive_factor = {
                static_cast<float>(gltf_material.emissiveFactor[0]),
                static_cast<float>(gltf_material.emissiveFactor[1]),
                static_cast<float>(gltf_material.emissiveFactor[2]),
            };
            if (gltf_material.emissiveTexture.index != -1)
            {
                // TODO: load emissive texture
                material->emissive_texture = nullptr;
            }

            const auto& pbr = gltf_material.pbrMetallicRoughness;

            // TODO: support A channel
            material->base_color_factor = {
                static_cast<float>(pbr.baseColorFactor[0]),
                static_cast<float>(pbr.baseColorFactor[1]),
                static_cast<float>(pbr.baseColorFactor[2]),
            };
            if (pbr.baseColorTexture.index != -1)
            {
                material->base_color_texture =
                    result->textures.at(pbr.baseColorTexture.index).get();
            }
        }

        // load mesh
        for (const auto& gltf_mesh : gltf_model.meshes)
        {
            USAMI_REQUIRE(gltf_mesh.primitives.size() == 1);
            const auto& gltf_primitive = gltf_mesh.primitives.front();

            auto& mesh = result->meshes.emplace_back(make_unique<SceneMesh>());

            mesh->name     = gltf_mesh.name;
            mesh->material = result->materials[gltf_primitive.material].get();

            // load indices
            {
                const auto& gltf_accessor = gltf_model.accessors.at(gltf_primitive.indices);
                const auto& gltf_bufview  = gltf_model.bufferViews.at(gltf_accessor.bufferView);
                const auto& gltf_buffer   = gltf_model.buffers.at(gltf_bufview.buffer);

                USAMI_REQUIRE(gltf_accessor.type == TINYGLTF_TYPE_SCALAR);
                USAMI_REQUIRE(gltf_accessor.componentType ==
                              TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT);

                // allocate buffer
                auto& buffer =
                    result->buffers.emplace_back(AssignConstructBuffer<uint32_t, uint16_t, 1>(
                        gltf_accessor, gltf_bufview, gltf_buffer));

                USAMI_ASSERT(gltf_accessor.count % 3 == 0);
                mesh->num_face = gltf_accessor.count / 3;
                mesh->indices =
                    SceneDataBufferView<uint32_t, 1>{.buffer = buffer.get(), .offset = 0};
            }

            // load vertex position
            {
                auto iter = gltf_primitive.attributes.find("POSITION");
                USAMI_REQUIRE(iter != gltf_primitive.attributes.end());

                const auto& gltf_accessor = gltf_model.accessors.at(iter->second);
                const auto& gltf_bufview  = gltf_model.bufferViews.at(gltf_accessor.bufferView);
                const auto& gltf_buffer   = gltf_model.buffers.at(gltf_bufview.buffer);

                USAMI_REQUIRE(gltf_accessor.type == TINYGLTF_TYPE_VEC3);
                USAMI_REQUIRE(gltf_accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);

                // allocate buffer
                auto& buffer = result->buffers.emplace_back(
                    CopyConstructBuffer<float, 3>(gltf_accessor, gltf_bufview, gltf_buffer));

                mesh->vertices = SceneDataBufferView<float, 3>{.buffer = buffer.get(), .offset = 0};
            }

            // load vertex normal
            {
                auto iter = gltf_primitive.attributes.find("NORMAL");
                if (iter != gltf_primitive.attributes.end())
                {
                    const auto& gltf_accessor = gltf_model.accessors.at(iter->second);
                    const auto& gltf_bufview  = gltf_model.bufferViews.at(gltf_accessor.bufferView);
                    const auto& gltf_buffer   = gltf_model.buffers.at(gltf_bufview.buffer);

                    USAMI_REQUIRE(gltf_accessor.type == TINYGLTF_TYPE_VEC3);
                    USAMI_REQUIRE(gltf_accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);

                    // allocate buffer
                    auto& buffer = result->buffers.emplace_back(
                        CopyConstructBuffer<float, 3>(gltf_accessor, gltf_bufview, gltf_buffer));

                    mesh->normals =
                        SceneDataBufferView<float, 3>{.buffer = buffer.get(), .offset = 0};
                }
                else
                {
                    mesh->normals = SceneDataBufferView<float, 3>{.buffer = nullptr, .offset = 0};
                }
            }

            // load vertex texture coordinate
            {
                auto iter = gltf_primitive.attributes.find("TEXCOORD_0");
                if (iter != gltf_primitive.attributes.end())
                {
                    const auto& gltf_accessor = gltf_model.accessors.at(iter->second);
                    const auto& gltf_bufview  = gltf_model.bufferViews.at(gltf_accessor.bufferView);
                    const auto& gltf_buffer   = gltf_model.buffers.at(gltf_bufview.buffer);

                    USAMI_REQUIRE(gltf_accessor.type == TINYGLTF_TYPE_VEC2);
                    USAMI_REQUIRE(gltf_accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);

                    // allocate buffer
                    auto& buffer = result->buffers.emplace_back(
                        CopyConstructBuffer<float, 2>(gltf_accessor, gltf_bufview, gltf_buffer));

                    mesh->tex_coords =
                        SceneDataBufferView<float, 2>{.buffer = buffer.get(), .offset = 0};
                }
                else
                {
                    mesh->tex_coords =
                        SceneDataBufferView<float, 2>{.buffer = nullptr, .offset = 0};
                }
            }
        }

        // load scene node
        for (size_t i = 0; i < gltf_model.nodes.size(); ++i)
        {
            result->nodes.push_back(make_unique<SceneNode>());
        }

        for (size_t i = 0; i < gltf_model.nodes.size(); ++i)
        {
            const auto& gltf_node = gltf_model.nodes[i];
            SceneNode* node       = result->nodes[i].get();

            node->name = gltf_node.name;

            if (gltf_node.mesh != -1)
            {
                node->mesh = result->meshes.at(gltf_node.mesh).get();
            }

            if (!gltf_node.matrix.empty())
            {
                std::array<float, 16> mat_data;
                for (size_t i = 0; i < 16; ++i)
                {
                    mat_data[i] = static_cast<float>(gltf_node.matrix[i]);
                }

                // NOTE matrix in gltf is column-major while ours are row-major
                node->transform = Matrix4{mat_data}.Transpose();
            }

            for (int gltf_child_node_index : gltf_node.children)
            {
                node->children.push_back(result->nodes[gltf_child_node_index].get());
            }
        }

        // load scene roots
        {
            USAMI_REQUIRE(gltf_model.scenes.size() == 1);
            const auto& gltf_scene = gltf_model.scenes.front();

            result->name = gltf_scene.name;
            for (int node_id : gltf_scene.nodes)
            {
                result->roots.push_back(result->nodes.at(node_id).get());
            }
        }

        return result;
    }

} // namespace usami
