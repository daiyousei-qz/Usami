#pragma once
#include "usami/ray/light.h"

namespace usami::ray
{
    class SpotLight : public Light
    {
    private:
        Vec3f point_;
        Vec3f direction_;
        float cos_theta_;

        // radiance at unit sphere around the point
        SpectrumRGB intensity_;

    public:
        SpotLight(const Vec3f& point, const Vec3f& direction, float theta,
                  const SpectrumRGB& intensity)
            : Light(LightType::DeltaPoint), point_(point), direction_(direction.Normalize()),
              cos_theta_(Cos(theta)), intensity_(intensity)
        {
        }

        SpectrumRGB Eval(const Ray& ray) const override
        {
            if (-Dot(ray.d, direction_) < cos_theta_)
            {
                return 0.f;
            }

            return intensity_ / (point_ - ray.o).LengthSq();
            return 0.f;
        }

        LightSample Sample(const IntersectionInfo& isect, const Point2f& u) const override
        {
            Vec3f wi = point_;
            SpectrumRGB radiance =
                -Dot(wi, direction_) > cos_theta_ ? intensity_ / wi.LengthSq() : 0.f;

            return LightSample{wi.Normalize(), point_, radiance, 1.f, LightType::DeltaPoint};
        }

        SpectrumRGB Power() const override
        {
            return intensity_ * AreaUnitCone(cos_theta_);
        }
    };
} // namespace usami::ray