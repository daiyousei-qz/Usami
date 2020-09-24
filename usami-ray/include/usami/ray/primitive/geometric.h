#pragma once
#include "usami/ray/ray.h"
#include "usami/ray/primitive.h"

namespace usami::ray
{
    template <GeometricShape Geometry>
    class GeometricPrimitive : public Primitive
    {
    public:
        GeometricPrimitive(Geometry geometry, bool reverse_orientation = false)
            : geometry_(geometry), reverse_orientation_(reverse_orientation)
        {
        }

        float Area() const override
        {
            return geometry_.Area();
        }

        bool Intersect(const Ray& ray, float t_min, float t_max,
                       IntersectionInfo& isect) const override
        {
            bool hit = geometry_.Intersect(ray, t_min, t_max, isect);
            if (hit)
            {
                isect.object     = this;
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

        void SamplePoint(const Point2f& u, Vec3& p_out, Vec3& n_out, float& pdf_out) const override
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

        Material* GetMaterial() const noexcept
        {
            return material_.get();
        }
        AreaLight* GetAreaLight() const noexcept
        {
            return area_light_.get();
        }

    private:
        Geometry geometry_;
        bool reverse_orientation_;

        shared_ptr<Material> material_    = nullptr;
        unique_ptr<AreaLight> area_light_ = nullptr;
    };
} // namespace usami::ray
