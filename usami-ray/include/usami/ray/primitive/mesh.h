#pragma once
#include "usami/mesh.h"
#include "usami/math/math.h"
#include "usami/ray/ray.h"
#include "usami/ray/primitive.h"
#include "usami/ray/composite/bvh.h"
#include "usami/ray/material/diffuse.h"

namespace usami::ray
{
    class MeshTriangle;
    class MeshPrimitive;

    class MeshTriangle : public Primitive
    {
    private:
        MeshPrimitive* parent_;
        int iface_;

    public:
    };

    class MeshPrimitive : public Primitive
    {
    private:
        SceneMesh* mesh_;

        MeshBvhComposite bvh_;

        Matrix4 model_to_world_;
        Matrix4 world_to_model_;

    public:
        MeshPrimitive(SceneMesh* mesh, Matrix4 model_to_world)
            : mesh_(mesh), bvh_(MeshBvhComposite::PrimitiveCollectionType{mesh}),
              model_to_world_(model_to_world), world_to_model_(model_to_world.Inverse())
        {
        }

        float Area() const override
        {
            USAMI_NO_IMPL();
        }

        BoundingBox Bounding() const
        {
            return bvh_.Bounding();
        }

        bool Intersect(const Ray& ray, float t_min, float t_max, Workspace& ws,
                       IntersectionInfo& isect_out) const override
        {
            Ray ray_model{world_to_model_.ApplyPoint(ray.o),
                          world_to_model_.ApplyVector(ray.d).Normalize()};

            bool success = bvh_.Intersect(ray_model, t_min, t_max, ws, isect_out);
            if (success)
            {
                isect_out.point = ray.o + isect_out.t * ray.d;
                isect_out.ng    = model_to_world_.ApplyVector(isect_out.ng).Normalize();
                isect_out.ns    = model_to_world_.ApplyVector(isect_out.ns).Normalize();

                static shared_ptr<Material> mat_sphere =
                    make_shared<DiffuseMaterial>(Vec3f{.2f, .5f, .2f});
                isect_out.primitive = this;
                isect_out.material  = mat_sphere.get();
            }

            return success;
        }

        void SamplePoint(const Point2f& u, Vec3f& p_out, Vec3f& n_out,
                         float& pdf_out) const override
        {
            USAMI_NO_IMPL();
        }
    };
} // namespace usami::ray
