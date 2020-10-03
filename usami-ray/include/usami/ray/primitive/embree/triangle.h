#pragma once
#include "usami/ray/primitive.h"

namespace usami::ray
{
    /**
     * A placeholder primitive for a certain triangle
     * - a triangle that is associated with a light source
     * - a triangle that is hit by a ray
     */
    class EmbreeTriangle : public Primitive
    {
    private:
        friend class EmbreeScene;

        // index of associated geometry
        unsigned geom_id_;
        // index of the triangle
        unsigned prim_id_;

        // pre-computed triangle geometric information
        float area_;
        Vec3f v0_, e1_, e2_;

    public:
        float Area() const override
        {
            return area_;
        }

        BoundingBox Bounding() const noexcept
        {
            Vec3f v1 = v0_ + e1_;
            Vec3f v2 = v0_ + e2_;
            return BoundingBox{Min(v0_, Min(v1, v2)), Max(v0_, Max(v1, v2))};
        }

        bool Intersect(const Ray& ray, float t_min, float t_max,
                       IntersectionInfo& info) const override
        {
            // we don't need to implement this as this won't be called
            USAMI_NO_IMPL();
        }

        void SamplePoint(const Point2f& u, Vec3f& p_out, Vec3f& n_out,
                         float& pdf_out) const override
        {
            float t  = Sqrt(u[0]);
            float k1 = 1 - t;
            float k2 = u[1] * t;

            p_out   = v0_ + k1 * e1_ + k2 * e2_;
            n_out   = Cross(e1_, e2_).Normalize(); // TODO: should be scattering normal?
            pdf_out = 1.f / area_;
        }

        // TODO: ensure this is not often called
        bool Equals(const Primitive* other) const override
        {
            auto tri = dynamic_cast<const EmbreeTriangle*>(other);

            return tri && tri->geom_id_ == geom_id_ && tri->prim_id_ == prim_id_;
        }
    };
} // namespace usami::ray
