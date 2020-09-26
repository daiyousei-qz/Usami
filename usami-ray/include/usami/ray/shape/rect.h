#pragma once
#include "usami/math/math.h"
#include "usami/math/sampling.h"
#include "usami/ray/ray.h"

namespace usami::ray
{
    /**
     * A rectangle that is placed horizontally, paralell to xy plane
     */
    struct Rect
    {
    public:
        Vec3f center;
        float len_x, len_y;

    public:
        Rect(Vec3f center, float len_x, float len_y) : center(center), len_x(len_x), len_y(len_y)
        {
            USAMI_REQUIRE(len_x > 0 && len_y > 0);
        }

        float Area() const noexcept
        {
            return len_x * len_y;
        }

        bool Intersect(const Ray& ray, float t_min, float t_max,
                       IntersectionInfo& isect) const noexcept
        {
            // ray is paralell to the rect
            if (ray.d.z == 0)
            {
                return false;
            }

            // compute point P that ray hits at plane rect's plane
            float t = (center.z - ray.o.z) / ray.d.z;
            Vec3f P = ray.o + t * ray.d;

            if (t < t_min || t > t_max)
            {
                return false;
            }

            // test if hit point is in the rectangle
            Vec3f delta = P - center + 0.5f * Vec3f{len_x, len_y, 0};
            if (delta[0] < 0 || delta[0] > len_x || delta[1] < 0 || delta[1] > len_y)
            {
                return false;
            }

            isect.t     = t;
            isect.point = P;
            isect.ng    = {0, 0, 1};
            isect.ns    = {0, 0, 1};
            isect.uv    = {delta[0] / len_x, delta[1] / len_y};
            return true;
        }

        void SamplePoint(const Point2f& u, Vec3f& p_out, Vec3f& n_out,
                         float& pdf_out) const noexcept
        {
            p_out   = Vec3f{(u[0] - .5f) * len_x, (u[1] - .5f) * len_y, 0} + center;
            n_out   = {0, 0, 1};
            pdf_out = 1.f / Area();
        }
    };

    // NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Rect, center, len_x, len_y)
} // namespace usami::ray