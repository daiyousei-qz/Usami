#pragma once
#include "usami/ray/ray.h"
#include "usami/ray/primitive.h"

namespace usami::ray
{
    template <GeometricShape Geometry>
    class GeometricPrimitive : public Primitive
    {
    private:
        Geometry geometry_;
        bool reverse_orientation_;

        shared_ptr<Material> material_    = nullptr;
        unique_ptr<AreaLight> area_light_ = nullptr;

    public:
        GeometricPrimitive(Geometry geometry, bool reverse_orientation = false)
            : geometry_(geometry), reverse_orientation_(reverse_orientation)
        {
        }

        Material* GetMaterial() const noexcept
        {
            return material_.get();
        }
        AreaLight* GetAreaLight() const noexcept
        {
            return area_light_.get();
        }

        float Area() const override
        {
            return geometry_.Area();
        }

        BoundingBox Bounding() const override
        {
            return geometry_.Bounding();
        }

        bool Intersect(const Ray& ray, float t_min, float t_max, Workspace& ws,
                       IntersectionInfo& isect) const override
        {
            bool hit = geometry_.Intersect(ray, t_min, t_max, isect);
            if (hit)
            {
                isect.primitive  = this;
                isect.area_light = GetAreaLight();
                isect.material   = GetMaterial();

                if (reverse_orientation_)
                {
                    isect.ns = -isect.ns;
                    isect.ng = -isect.ng;
                    isect.uv = 1.f - isect.uv;
                }
            }

            return hit;
        }

        bool Intersect(const Ray& ray, float t_min, float t_max, Workspace& ws,
                       OcclusionInfo& occ_out) const override
        {
            float t;
            if (geometry_.Occlude(ray, t_min, t_max, t))
            {
                occ_out.t         = t;
                occ_out.primitive = this;
                return true;
            }

            return false;
        }

        void SamplePoint(const Point2f& u, Vec3f& p_out, Vec3f& n_out,
                         float& pdf_out) const override
        {
            geometry_.SamplePoint(u, p_out, n_out, pdf_out);

            if (reverse_orientation_)
            {
                n_out = -n_out;
            }
        }

        void BindMaterial(shared_ptr<Material> mat)
        {
            material_ = std::move(mat);
        }
        template <typename TLight, typename... TArgs>
        void BindAreaLight(TArgs&&... args) requires std::derived_from<TLight, AreaLight>
        {
            area_light_ = make_unique<TLight>(this, std::forward<TArgs>(args)...);
        }
    };
} // namespace usami::ray
