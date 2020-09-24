#pragma once
#include "usami/color.h"

namespace usami::ray
{
    /**
     * A coarse description of characteristics of a specific BSDF
     */
    class BsdfType
    {
    public:
        enum FlagType
        {
            None = 0,

            // major category: BRDF or BTDF
            Reflection   = 1,
            Transmission = 2,

            // sub-category
            Diffuse  = 4,
            Glossy   = 8,
            Specular = 16,

            // quick access
            Any                  = Reflection | Transmission | Diffuse | Glossy | Specular,
            DiffuseRefl          = Reflection | Diffuse,
            GlossyRefl           = Reflection | Glossy,
            SpecularRefl         = Reflection | Specular,
            SpecularTransmission = Reflection | Transmission | Specular
        };

        constexpr BsdfType(FlagType flag) : value_(flag)
        {
        }
        constexpr BsdfType(int flag) : BsdfType(static_cast<FlagType>(flag))
        {
        }

        constexpr BsdfType Also(BsdfType type) const noexcept
        {
            return BsdfType{value_ | type.value_};
        }

        constexpr bool Contain(BsdfType flag) const noexcept
        {
            return (value_ & flag.value_) != 0;
        }

        constexpr bool ContainReflection() const noexcept
        {
            return Contain(BsdfType::Reflection);
        }
        constexpr bool ContainTransmission() const noexcept
        {
            return Contain(BsdfType::Transmission);
        }

    private:
        FlagType value_ = None;
    };

    /**
     * Interface for a bidirectional scattering distribution function
     *
     * Note that
     * 1. surface normal vector is assumed to be (0, 0, 1), where volume above z-plane
     * is outside the surface and below is inside.
     * 2. incident direction `wi` used is actually the opposite of incident light direction for ease
     * of BSDF implementation.
     */
    class Bsdf : public UsamiObject
    {
    public:
        Bsdf() = default;
        Bsdf(BsdfType type) : type_(type)
        {
        }

        BsdfType GetType() const noexcept
        {
            return type_;
        }

        /**
         * Evaluate bidirectional scattering distribution function $f_s(wo, wi)$
         *
         * @return Ratio of radiance carried by `wi` that is scattered into `wo`
         */
        virtual SpectrumRGB Eval(const Vec3f& wo, const Vec3f& wi) const noexcept = 0;

        /**
         * Samples an incident direction `wi` and evaluate BSDF.
         *
         * @return Ratio of radiance carried by `wi` that is scattered into `wo`
         */
        virtual SpectrumRGB SampleAndEval(const Point2f& u, const Vec3f& wo, Vec3f& wi_out,
                                          float& pdf_out) const noexcept = 0;

        /**
         * For a specific outgoing direction `wo`, compute probability distribution of incident
         * direction `wi` to be sampled
         */
        virtual float Pdf(const Vec3f& wo, const Vec3f& wi) const noexcept = 0;

    protected:
        BsdfType type_ = BsdfType::None;
    };
} // namespace usami::ray
