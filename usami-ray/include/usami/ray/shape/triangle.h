#pragma once
#include "usami/math/math.h"
#include "usami/math/sampling.h"
#include "usami/ray/ray.h"

namespace usami::ray
{
    struct Triangle
    {
    public:
        Vec3f v0;
        Vec3f v1;
        Vec3f v2;

    public:
        Triangle(Vec3f v0, Vec3f v1, Vec3f v2) : v0(v0), v1(v1), v2(v2)
        {
        }

        float Area() const noexcept
        {
            return Cross(v1, v2).Length() * .5f;
        }

        bool Intersect(const Ray& ray, float t_min, float t_max,
                       IntersectionInfo& isect) const noexcept
        {
            Vec3f edge1 = v1 - v0;
            Vec3f edge2 = v2 - v0;

            Vec3f h = Cross(ray.d, edge2);
            float a = Dot(edge1, h);
            if (a > -kFloatEpsilon && a < kFloatEpsilon)
            {
                return false;
            }

            float f = 1.f / a;
            Vec3f s = ray.o - v0;
            float u = f * Dot(s, h);
            if (u < 0 || u > 1)
            {
                return false;
            }

            Vec3f q = Cross(s, edge1);
            float v = f * Dot(ray.d, q);
            if (v < 0 || u + v > 1)
            {
                return false;
            }

            float t = f * Dot(edge2, q);
            if (t < t_min || t > t_max)
            {
                return false;
            }

            isect.t     = t;
            isect.point = ray.o + t * ray.d;
            isect.ng    = Cross(edge1, edge2);
            isect.ns    = isect.ng;
            isect.uv    = {u, v};
            return true;
        }

        void SamplePoint(const Point2f& u, Vec3f& p_out, Vec3f& n_out,
                         float& pdf_out) const noexcept
        {
            USAMI_NO_IMPL();
        }
    };
} // namespace usami::ray
