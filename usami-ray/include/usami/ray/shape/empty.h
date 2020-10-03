#pragma once
#include "usami/math/math.h"
#include "usami/math/sampling.h"
#include "usami/ray/ray.h"
#include "usami/ray/bbox.h"

namespace usami::ray
{
    /**
     * An empty shape that doesn't exist in the space
     */
    class Empty
    {
    public:
        constexpr Empty() noexcept
        {
        }

        /**
         * Surface area for the geometric object
         */
        float Area() const noexcept
        {
            return 0.f;
        }

        /**
         * Axis-aligned bounding box of the geometric object
         */
        BoundingBox Bounding() const noexcept
        {
            return {0.f};
        }

        /**
         * Test if a ray intersect with the geometric object, fill `IntersectionInfo` if intersected
         */
        bool Intersect(const Ray& ray, float t_min, float t_max,
                       IntersectionInfo& isect) const noexcept
        {
            return false;
        }

        /**
         * Test if a ray intersect with the geometric object
         */
        bool Occlude(const Ray& ray, float t_min, float t_max, float& t_out) const noexcept
        {
            return false;
        }

        /**
         * Samples a point on the surface area
         */
        void SamplePoint(const Point2f& u, Vec3f& p_out, Vec3f& n_out,
                         float& pdf_out) const noexcept
        {
            p_out   = Vec3f{};
            n_out   = {0, 0, 0};
            pdf_out = 0.f;
        }
    };
} // namespace usami::ray
