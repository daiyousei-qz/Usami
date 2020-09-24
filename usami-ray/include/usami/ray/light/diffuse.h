#pragma once
#include "usami/ray/light.h"
#include "usami/ray/primitive.h"

namespace usami::ray
{
    class DiffuseAreaLight : public AreaLight
    {
    private:
        // radiance at surface area
        SpectrumRGB intensity_;

    public:
        DiffuseAreaLight(const Primitive* object, SpectrumRGB intensity)
            : AreaLight(object), intensity_(intensity)
        {
        }

        SpectrumRGB Eval(const Ray& ray) const override
        {
            return intensity_;
        }

        LightSample Sample(const IntersectionInfo& isect, const Point2f& u) const override
        {
            Vec3f point;
            Vec3f normal;
            float pdf;
            GetPrimitive()->SamplePoint(u, point, normal, pdf);

            Vec3f wi       = point - isect.point;
            Vec3f radiance = Dot(wi, normal) < 0 ? intensity_ : 0.f;

            return LightSample{wi.Normalize(), point, radiance, pdf, LightType::Area};
        }

        SpectrumRGB Power() const override
        {
            return intensity_ * kPi * GetPrimitive()->Area();
        }
    };
} // namespace usami::ray
