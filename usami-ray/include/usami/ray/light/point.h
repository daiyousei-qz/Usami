#pragma once
#include "usami/ray/light.h"

namespace usami::ray
{
    class PointLight : public Light
    {
    private:
        // position of point light source
        Vec3f point_;

        // radiance at unit sphere around the point
        SpectrumRGB intensity_;

    public:
        PointLight(const Vec3f& p, const Vec3f& intensity)
            : Light(LightType::DeltaPoint), point_(p), intensity_(intensity)
        {
        }

        SpectrumRGB Eval(const Ray& ray) const override
        {
            return 0.f;
        }

        LightSample Sample(const IntersectionInfo& isect, const Point2f& u) const override
        {
            Vec3f wi             = point_ - isect.point;
            SpectrumRGB radiance = intensity_ / wi.LengthSq();

            return LightSample{wi.Normalize(), point_, radiance, 1.f, LightType::DeltaPoint};
        }

        SpectrumRGB Power() const override
        {
            return intensity_ * kAreaUnitSphere;
        }
    };
} // namespace usami::ray