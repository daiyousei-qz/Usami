#pragma once
#include "usami/math/math.h"
#include "usami/math/sampling.h"
#include "usami/ray/ray.h"

namespace usami::ray
{
    struct Sphere
    {
    public:
        Vec3f center;
        float radius;

    public:
        Sphere(Vec3f center, float radius) : center(center), radius(radius)
        {
        }

        float Area() const noexcept
        {
            return 4.f * kPi * radius;
        }

        bool Intersect(const Ray& ray, float t_min, float t_max,
                       IntersectionInfo& isect) const noexcept
        {
            // ray P(t) = A + t*B
            // sphere | P - C |^2 = r^2
            Vec3f A = ray.o;
            Vec3f B = ray.d;
            Vec3f C = center;
            Vec3f D = A - C;
            float r = radius;

            // equation
            // |P(t) - C|^2 = |A + t*B - C|^2 = r^2, D = A - C
            // ...
            // a*t^2 + b*t + c = 0
            // a = dot(B, B)
            // b = 2*dot(B, D)
            // c = dot(D, D) - r^2
            float a = Dot(B, B);
            float b = 2 * Dot(B, D);
            float c = Dot(D, D) - r * r;

            // no solution
            float delta_sq = b * b - 4 * a * c;
            if (delta_sq < 0)
            {
                return false;
            }

            // has solution
            float delta = Sqrt(delta_sq);
            float t0    = (-b - delta) / (2 * a);
            float t1    = (-b + delta) / (2 * a);

            float t = t0 >= 0 ? t0 : t1;
            if (t < 0 || t < t_min || t > t_max)
            {
                return false;
            }

            Vec3f P      = A + t * B;
            Vec3f normal = (P - C).Normalize();
            float u      = 1 - std::atan2(normal.y, normal.x) * kInvTwoPi;
            float v      = 1 - std::acos(normal.z) * kInvPi;
            if (u < 0)
            {
                u += 1;
            }

            isect.t     = t;
            isect.point = P;
            isect.ng    = normal;
            isect.ns    = normal;
            isect.uv    = {u, v};
            return true;
        }

        void SamplePoint(const Point2f& u, Vec3f& p_out, Vec3f& n_out,
                         float& pdf_out) const noexcept
        {
            n_out   = SampleUniformSphere(u);
            p_out   = n_out * radius + center;
            pdf_out = 1.f / Area();
        }
    };

    // NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Sphere, center, radius)
} // namespace usami::ray