#pragma once
#include "usami/math/sampling.h"
#include "usami/ray/bsdf.h"
#include "usami/ray/bsdf/bsdf_geometry.h"

namespace usami::ray
{
    class SpecularReflection : public Bsdf
    {
    private:
        SpectrumRGB albedo_;

    public:
        SpecularReflection(SpectrumRGB albedo) : Bsdf(BsdfType::SpecularRefl), albedo_(albedo)
        {
        }

        SpectrumRGB Eval(const Vec3f& wo, const Vec3f& wi) const noexcept override
        {
            return 0.f;
        }

        SpectrumRGB SampleAndEval(const Point2f& u, const Vec3f& wo, Vec3f& wi_out,
                                  float& pdf_out) const noexcept override
        {
            wi_out  = ReflectRayQuick(wo);
            pdf_out = 1.f;
            return albedo_;
        }

        float Pdf(const Vec3f& wi, const Vec3f& wo) const noexcept override
        {
            return 0;
        }
    };

    class SpecularTransmission : public Bsdf
    {
    private:
        float eta_in_;
        float eta_out_;
        SpectrumRGB albedo_;

    public:
        SpectrumRGB Eval(const Vec3f& wo, const Vec3f& wi) const noexcept override
        {
            return 0.f;
        }

        SpectrumRGB SampleAndEval(const Point2f& u, const Vec3f& wo, Vec3f& wi_out,
                                  float& pdf_out) const noexcept override
        {
            USAMI_NO_IMPL();
        }

        float Pdf(const Vec3f& wi, const Vec3f& wo) const noexcept override
        {
            return 0;
        }
    };
} // namespace usami::ray
