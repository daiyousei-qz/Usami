#pragma once
#include "usami/math/math.h"
#include "usami/math/sampling.h"
#include "usami/ray/ray.h"
#include "usami/ray/bbox.h"

namespace usami::ray::shape
{
    struct Triangle
    {
    public:
        Vec3f v0;
        Vec3f e1;
        Vec3f e2;

    public:
        Triangle(Vec3f v0, Vec3f v1, Vec3f v2) : v0(v0), e1(v1 - v0), e2(v2 - v0)
        {
        }

        float Area() const noexcept
        {
            return Cross(e1, e2).Length() * .5f;
        }

        BoundingBox Bounding() const noexcept
        {
            Vec3f v1 = v0 + e1;
            Vec3f v2 = v0 + e2;
            return BoundingBox{Min(v0, Min(v1, v2)), Max(v0, Max(v1, v2))};
        }

        bool Intersect(const Ray& ray, float t_min, float t_max,
                       IntersectionInfo& isect) const noexcept
        {
            Vec3f h = Cross(ray.d, e2);
            float a = Dot(e1, h);
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

            Vec3f q = Cross(s, e1);
            float v = f * Dot(ray.d, q);
            if (v < 0 || u + v > 1)
            {
                return false;
            }

            float t = f * Dot(e2, q);
            if (t < t_min || t > t_max)
            {
                return false;
            }

            isect.t     = t;
            isect.point = ray.o + t * ray.d;
            isect.ng    = Cross(e1, e2);
            isect.ns    = isect.ng;
            isect.uv    = {u, v};
            return true;
        }

        bool Occlude(const Ray& ray, float t_min, float t_max, float& t_out) const noexcept
        {
            Vec3f h = Cross(ray.d, e2);
            float a = Dot(e1, h);
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

            Vec3f q = Cross(s, e1);
            float v = f * Dot(ray.d, q);
            if (v < 0 || u + v > 1)
            {
                return false;
            }

            float t = f * Dot(e2, q);
            if (t < t_min || t > t_max)
            {
                return false;
            }

            t_out = t;
            return true;
        }

        void SamplePoint(const Point2f& u, Vec3f& p_out, Vec3f& n_out,
                         float& pdf_out) const noexcept
        {
            float t       = Sqrt(u[0]);
            Vec3f n_sized = Cross(e1, e2); // len = area of the formed parallelogram
            float len_inv = 1.f / n_sized.Length();

            p_out   = v0 + t * e1 + (1 - t) * e2;
            n_out   = n_sized * len_inv;
            pdf_out = 1.f * .5f * len_inv;
        }
    };
} // namespace usami::ray::shape
