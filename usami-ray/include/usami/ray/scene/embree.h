#pragma once
#include "usami/model.h"
#include "usami/mesh.h"
#include "usami/ray/scene.h"
#include "usami/ray/primitive/embree/triangle.h"
#include <embree3/rtcore.h>

namespace usami::ray
{
    class EmbreeMeshGeometry
    {
    private:
        int id_;

        const SceneMesh* mesh_;

        Matrix4 model_to_world_;

        const Material* material                  = nullptr;
        std::vector<const AreaLight*> area_lights = {};

        friend class EmbreeScene;

    public:
        int Id() const noexcept
        {
            return id_;
        }

        const SceneMesh& Mesh() const noexcept
        {
            return *mesh_;
        }

        // model to world transformation
        const Matrix4& Transform() const noexcept
        {
            return model_to_world_;
        }

        bool ContainAreaLight() const noexcept
        {
            return !area_lights.empty();
        }
        const AreaLight* GetAreaLight(int prim_id) const noexcept
        {
            if (!area_lights.empty())
            {
                return area_lights[prim_id];
            }
            else
            {
                return nullptr;
            }
        }
        const Material* GetMaterial() const noexcept
        {
            return material;
        }
    };

    struct EmbreeRegisteredModel
    {
        shared_ptr<SceneModel> model;

        std::vector<EmbreeMeshGeometry*> geometries;

        std::unordered_map<const SceneMesh*, RTCScene> mesh_instance_cache;

        std::unordered_map<const SceneMaterial*, Material*> material_cache;
    };

    class EmbreeScene : public Scene
    {
    private:
        Arena arena_;

        RTCScene scene_;
        std::vector<EmbreeRegisteredModel> models_;
        std::vector<EmbreeMeshGeometry*> geom_lookup_;

    public:
        EmbreeScene();
        ~EmbreeScene();

        void Commit() override;

        bool Intersect(const Ray& ray, Workspace& workspace,
                       IntersectionInfo& isect) const override;

        void AddModel(shared_ptr<SceneModel> model,
                      const Matrix4& model_to_world = Matrix4::Identity());

        void AddInfiniteAreaLight(shared_ptr<Texture2D<Vec3f>> tex, float intensity,
                                  Vec3f world_center, float world_radius)
        {
            SetGlobalLightSource(arena_.Construct<InfiniteAreaLight>(std::move(tex), intensity,
                                                                     world_center, world_radius));
        }

    private:
        void AddSceneNode(const SceneNode* node, const Matrix4& parent_transform,
                          const EmbreeRegisteredModel& registry);
        void AddMeshGeometry(const SceneMesh* mesh, const Matrix4& model_to_world,
                             const EmbreeRegisteredModel& registry);

        void RegisterMeshGeometry(const EmbreeMeshGeometry& geometry,
                                  const Matrix4& model_to_world);

        Primitive* InstantiatePrimitive(unsigned geom_id, unsigned prim_id,
                                        const TriangleDesc& tri_desc);
        Primitive* InstantiateTemporaryPrimitive(Workspace& workspace, unsigned geom_id,
                                                 unsigned prim_id,
                                                 const TriangleDesc& tri_desc) const;

        void CreatePrimitiveAux(EmbreeTriangle& p, unsigned geom_id, unsigned prim_id,
                                const TriangleDesc& tri_desc) const;
    };
} // namespace usami::ray
