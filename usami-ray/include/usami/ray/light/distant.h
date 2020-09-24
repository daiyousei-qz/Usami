#pragma once
#include "usami/math/sampling.h"
#include "usami/ray/light.h"
#include "usami/ray/ray.h"

namespace usami::ray
{
    /**
     * Parallel light that is casted inifinitely away
     */
    class DistantLight : public Light
    {
    private:
        Vec3f direction_;
        SpectrumRGB intensity_;

        Vec3f world_center_;
        float world_radius_;

    public:
        DistantLight(const Vec3f& direction, const SpectrumRGB& intensity, Vec3f world_center,
                     float world_radius)
            : Light(LightType::DeltaDirection), direction_(direction.Normalize()),
              intensity_(intensity), world_center_(world_center), world_radius_(world_radius)
        {
        }

        SpectrumRGB Eval(const Ray& ray) const override
        {
            return 0.f;
        }

        LightSample Sample(const IntersectionInfo& isect, const Point2f& u) const override
        {
            return LightSample{-direction_, 0.f, intensity_, 1.f, LightType::DeltaDirection};
        }

        SpectrumRGB Power() const override
        {
            return intensity_ * kPi * world_radius_ * world_radius_;
        }
    };
} // namespace usami::ray
