#pragma once
#include "usami/math/sampling.h"
#include "usami/ray/bsdf.h"
#include "usami/ray/bsdf/bsdf_geometry.h"

namespace usami::ray
{
    class LambertianReflection : public Bsdf
    {
    private:
        SpectrumRGB albedo_;

    public:
        LambertianReflection(SpectrumRGB albedo) : Bsdf(BsdfType::DiffuseRefl), albedo_(albedo)
        {
        }

        SpectrumRGB Eval(const Vec3f& wo, const Vec3f& wi) const noexcept override
        {
            if (!SameHemisphere(wo, wi))
            {
                return 0.f;
            }

            return albedo_ / kPi;
        }

        SpectrumRGB SampleAndEval(const Point2f& u, const Vec3f& wo, Vec3f& wi_out,
                                  float& pdf_out) const noexcept
        {
            // cosine-weighted sample from bsdf hemisphere
            Vec3f wi = SampleCosineWeightedHemisphere(u);

            // correct direction if wo comes from inside of primitive
            float abs_wi_z = wi.z;
            if (wo.z < 0)
            {
                wi.z = -wi.z;
            }

            wi_out  = wi;
            pdf_out = PdfCosineWeightedHemisphere(abs_wi_z);
            return albedo_ / kPi;
        }

        float Pdf(const Vec3f& wi, const Vec3f& wo) const noexcept
        {
            if (!SameHemisphere(wo, wi))
            {
                return 0.f;
            }

            return PdfCosineWeightedHemisphere(Abs(wi.z));
        }
    };
} // namespace usami::ray
