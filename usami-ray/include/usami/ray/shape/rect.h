#pragma once
#include "usami/math/math.h"
#include "usami/math/sampling.h"
#include "usami/ray/ray.h"
#include "usami/ray/bbox.h"

namespace usami::ray::shape
{
    /**
     * A rectangle that is placed horizontally, paralell to xy plane
     */
    struct Rect
    {
    public:
        Vec3f p_minxy;
        float len_x, len_y;

    public:
        Rect(Vec3f center, float len_x, float len_y)
            : p_minxy(center - 0.5f * Vec3f{len_x, len_y, 0}), len_x(len_x), len_y(len_y)
        {
            USAMI_REQUIRE(len_x > 0 && len_y > 0);
        }

        float Area() const noexcept
        {
            return len_x * len_y;
        }

        BoundingBox Bounding() const noexcept
        {
            return BoundingBox{p_minxy, p_minxy + Vec3f{len_x, len_y, 0}};
        }

        template <bool ComputeGeometryInfo>
        bool IntersectTest(const Ray& ray, float t_min, float t_max, float* t_out, Vec3f* p_out,
                           Vec3f* n_out, Vec2f* uv_out) const
        {
            // ray is paralell to the rect
            if (ray.d.z == 0)
            {
                return false;
            }

            // compute point p that ray hits at plane rect's plane
            float t = (p_minxy.z - ray.o.z) / ray.d.z;
            Vec3f p = ray.o + t * ray.d;

            if (t < t_min || t > t_max)
            {
                return false;
            }

            // test if hit point is in the rectangle
            Vec3f delta = p - p_minxy;
            if (delta[0] < 0 || delta[0] > len_x || delta[1] < 0 || delta[1] > len_y)
            {
                return false;
            }

            *t_out = t;

            if constexpr (ComputeGeometryInfo)
            {
                *p_out  = p;
                *n_out  = {0, 0, 1};
                *uv_out = {delta[0] / len_x, delta[1] / len_y};
            }

            return true;
        }

        void SamplePoint(const Point2f& u, Vec3f& p_out, Vec3f& n_out,
                         float& pdf_out) const noexcept
        {
            p_out   = p_minxy + Vec3f{u[0] * len_x, u[1] * len_y, 0};
            n_out   = {0, 0, 1};
            pdf_out = 1.f / Area();
        }
    };

    // NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Rect, center, len_x, len_y)
} // namespace usami::ray::shape