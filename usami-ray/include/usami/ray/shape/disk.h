#pragma once
#include "usami/math/math.h"
#include "usami/math/sampling.h"
#include "usami/ray/ray.h"
#include "usami/ray/bbox.h"

namespace usami::ray
{
    /**
     * A disk that is placed horizontally, paralell to xy plane
     */
    struct Disk
    {
    public:
        Vec3f center;
        float radius;

    public:
        Disk(Vec3f center, float radius) : center(center), radius(radius)
        {
            USAMI_ASSERT(radius > 0);
        }

        float Area() const noexcept
        {
            return 2.f * kPi * radius;
        }

        BoundingBox Bounding() const noexcept
        {
            Vec3f radius_offset{radius, radius, 0};
            return BoundingBox{center - radius_offset, center + radius_offset};
        }

        bool Intersect(const Ray& ray, float t_min, float t_max,
                       IntersectionInfo& isect) const noexcept
        {
            // ray is paralell to the disk
            if (ray.d.z == 0)
            {
                return false;
            }

            // compute point p that ray hits at plane disk's plane
            float t = (center.z - ray.o.z) / ray.d.z;
            Vec3f p = ray.o + t * ray.d;

            if (t < t_min || t > t_max)
            {
                return false;
            }

            // test if hit point is in the circle
            Vec3f delta   = p - center;
            float dist_sq = delta.LengthSq();
            if (dist_sq > radius * radius)
            {
                return false;
            }

            // compute uv
            float phi = std::atan2(delta[1], delta[0]);
            if (phi < 0)
            {
                phi += kTwoPi;
            }

            float u = phi / kTwoPi;
            float v = (radius - Sqrt(dist_sq)) / radius;

            isect.t     = t;
            isect.point = p;
            isect.ng    = {0, 0, 1};
            isect.ns    = {0, 0, 1};
            isect.uv    = {u, v};
            return true;
        }

        bool Occlude(const Ray& ray, float t_min, float t_max, float& t_out) const noexcept
        {
            // ray is paralell to the disk
            if (ray.d.z == 0)
            {
                return false;
            }

            // compute point p that ray hits at plane disk's plane
            float t = (center.z - ray.o.z) / ray.d.z;
            Vec3f p = ray.o + t * ray.d;

            if (t < t_min || t > t_max)
            {
                return false;
            }

            // test if hit point is in the circle
            Vec3f delta   = p - center;
            float dist_sq = delta.LengthSq();
            if (dist_sq > radius * radius)
            {
                return false;
            }

            t_out = t;
            return true;
        }

        void SamplePoint(const Point2f& u, Vec3f& p_out, Vec3f& n_out,
                         float& pdf_out) const noexcept
        {
            p_out   = SampleUniformDisk(u) * radius + center;
            n_out   = {0, 0, 1};
            pdf_out = 1.f / Area();
        }
    };

    // NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Disk, center, radius)
} // namespace usami::ray