#pragma once
#include "usami/model.h"
#include "usami/mesh.h"
#include "usami/ray/scene.h"
#include "usami/ray/primitive/embree/triangle.h"
#include <embree3/rtcore.h>

namespace usami::ray
{
    class ModelRegistration
    {
    private:
        shared_ptr<SceneModel> model_;

        std::unordered_map<SceneMaterial*, Material*> material_cache_;
    };

    class EmbreeMeshGeometry
    {
    private:
        SceneMesh* mesh_;

        Matrix4 model_to_world_;

        const Material* material;
        std::vector<const AreaLight*> area_lights;

        friend class EmbreeScene;

    public:
        const SceneMesh& Mesh() const noexcept
        {
            return *mesh_;
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

    class EmbreeScene : public Scene
    {
    private:
        Arena arena_;

        RTCScene scene_;
        std::vector<EmbreeMeshGeometry*> geoms_;

    public:
        EmbreeScene();
        ~EmbreeScene();

        void Commit() override;

        bool Intersect(const Ray& ray, Workspace& workspace,
                       IntersectionInfo& isect) const override;

        void AddModel(shared_ptr<SceneModel> model,
                      const Matrix4& model_to_world = Matrix4::Identity());

    private:
        void AddSceneNode(const SceneNode* node, const Matrix4& transform);
        void AddMeshGeometry(const SceneMesh* mesh, const Matrix4& transform);

        uint32_t RegisterMeshGeometry(EmbreeMeshGeometry* geometry, const Matrix4& model_to_world);

        Primitive* InstantiatePrimitive(unsigned geom_id, unsigned prim_id,
                                        const TriangleDesc& tri_desc);
        Primitive* InstantiateTemporaryPrimitive(Workspace& workspace, unsigned geom_id,
                                                 unsigned prim_id,
                                                 const TriangleDesc& tri_desc) const;

        void CreatePrimitiveAux(EmbreeTriangle& p, unsigned geom_id, unsigned prim_id,
                                const TriangleDesc& tri_desc) const;
    };
} // namespace usami::ray
