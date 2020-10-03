#pragma once
#include "usami/math/math.h"
#include "usami/ray/ray.h"

namespace usami::ray
{
    struct BoundingBox
    {
    public:
        Vec3f p_min;
        Vec3f p_max;

    public:
        BoundingBox(Vec3f p) : p_min(p), p_max(p)
        {
        }
        BoundingBox(Vec3f p_min, Vec3f p_max) : p_min(p_min), p_max(p_max)
        {
        }

        Vec3f Extents() const noexcept
        {
            return p_max - p_min;
        }

        float Area() const noexcept
        {
            Vec3f extents = p_max - p_min;
            return 2 *
                   (extents[0] * extents[1] + extents[0] * extents[2] + extents[1] * extents[2]);
        }

        Vec3f Centroid() const noexcept
        {
            return (p_min + p_max) * .5f;
        }

        bool Occlude(const Ray& ray, float t_min, float t_max, float& t_out) const noexcept
        {
            Vec3f dtdd    = 1.f / ray.d;
            Vec3f t_p_min = (p_min - ray.o) * dtdd;
            Vec3f t_p_max = (p_max - ray.o) * dtdd;

            float t_hit = t_max;

            if (ray.d.X() != 0)
            {
                // hit plane x = p_min.x
                if (t_p_min.X() > t_min && t_p_min.X() < t_hit)
                {
                    Vec3f p = ray.o + t_p_min.X() * ray.d;
                    if (p.Y() >= p_min.Y() && p.Y() <= p_max.Y() && p.Z() >= p_min.Z() &&
                        p.Z() <= p_max.Z())
                    {
                        t_hit = t_p_min.X();
                    }
                }

                // hit plane x = p_max.x
                if (t_p_max.X() > t_min && t_p_max.X() < t_hit)
                {
                    Vec3f p = ray.o + t_p_max.X() * ray.d;
                    if (p.Y() >= p_min.Y() && p.Y() <= p_max.Y() && p.Z() >= p_min.Z() &&
                        p.Z() <= p_max.Z())
                    {
                        t_hit = t_p_max.X();
                    }
                }
            }

            if (ray.d.Y() != 0)
            {
                // hit plane y = p_min.y
                if (t_p_min.Y() > t_min && t_p_min.Y() < t_hit)
                {
                    Vec3f p = ray.o + t_p_min.Y() * ray.d;
                    if (p.X() >= p_min.X() && p.X() <= p_max.X() && p.Z() >= p_min.Z() &&
                        p.Z() <= p_max.Z())
                    {
                        t_hit = t_p_min.Y();
                    }
                }

                // hit plane y = p_max.y
                if (t_p_max.Y() > t_min && t_p_max.Y() < t_hit)
                {
                    Vec3f p = ray.o + t_p_max.Y() * ray.d;
                    if (p.X() >= p_min.X() && p.X() <= p_max.X() && p.Z() >= p_min.Z() &&
                        p.Z() <= p_max.Z())
                    {
                        t_hit = t_p_max.Y();
                    }
                }
            }

            if (ray.d.Z() != 0)
            {
                // hit plane z = p_min.z
                if (t_p_min.Z() > t_min && t_p_min.Z() < t_hit)
                {
                    Vec3f p = ray.o + t_p_min.Z() * ray.d;
                    if (p.X() >= p_min.X() && p.X() <= p_max.X() && p.Y() >= p_min.Y() &&
                        p.Y() <= p_max.Y())
                    {
                        t_hit = t_p_min.Z();
                    }
                }

                // hit plane z = p_max.z
                if (t_p_max.Z() > t_min && t_p_max.Z() < t_hit)
                {
                    Vec3f p = ray.o + t_p_max.Z() * ray.d;
                    if (p.X() >= p_min.X() && p.X() <= p_max.X() && p.Y() >= p_min.Y() &&
                        p.Y() <= p_max.Y())
                    {
                        t_hit = t_p_max.Z();
                    }
                }
            }

            if (t_hit == t_max)
            {
                return false;
            }

            t_out = t_hit;
            return true;
        }
    };

    inline BoundingBox UnionBBox(const BoundingBox& a, const BoundingBox& b)
    {
        return BoundingBox{Min(a.p_min, b.p_min), Max(a.p_max, b.p_max)};
    }
} // namespace usami::ray
