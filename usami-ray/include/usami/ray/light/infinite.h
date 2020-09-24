#pragma once
#include "usami/ray/light.h"
#include "usami/math/sampling.h"
#include "usami/texture.h"

namespace usami::ray
{
    class InfiniteAreaLight : public Light
    {
    private:
        shared_ptr<Texture2D<Vec3f>> tex_;
        float intensity_;

        Vec3f world_center_;
        float world_radius_;

    public:
        InfiniteAreaLight(shared_ptr<Texture2D<Vec3f>> tex, float intensity, Vec3f world_center,
                          float world_radius)
            : Light(LightType::Infinite), tex_(tex), intensity_(intensity),
              world_center_(world_center), world_radius_(world_radius)
        {
        }

        SpectrumRGB Eval(const Vec3f& wi_world) const
        {
            // TODO: reuse this with sphere shape
            // NOTE normal == wi_world
            float u = 0.5f + atan2(-wi_world.Z(), -wi_world.X()) * kInvTwoPi;
            float v = 0.5f - asin(-wi_world.Y()) * kInvPi;

            return intensity_ * tex_->Eval({u, v}, 0.f, 0.f);
        }

        SpectrumRGB Eval(const Ray& ray) const override
        {
            return Eval(ray.d);
        }

        LightSample Sample(const IntersectionInfo& isect, const Point2f& u) const override
        {
            Vec3f wi = SampleUniformSphere(u);
            if (Dot(wi, isect.ns) < 0)
            {
                wi = -wi;
            }

            return LightSample{wi, 0.f, Eval(wi), PdfUniformSphere() * 2, LightType::Infinite};
        }

        SpectrumRGB Power() const override
        {
            // TODO: verify this
            return intensity_ * kPi * world_radius_ * world_radius_;
        }
    };
} // namespace usami::ray
