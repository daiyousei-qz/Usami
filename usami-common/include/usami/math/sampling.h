#pragma once
#include "usami/math/math.h"

// facilities to map unit sampling to various dimensions
namespace usami
{
    /**
     * Samples a point on unit hemisphere
     *
     * @param u uniform sample in unit square
     */
    inline Vec3f SampleUniformHemisphere(Point2f u) noexcept
    {
        float z   = u[0];
        float r   = Sqrt(Max(0.f, 1 - z * z)); // avoid float error
        float phi = 2 * kPi * u[1];

        return Vec3f{r * Cos(phi), r * Sin(phi), z};
    }

    inline float PdfUniformHemisphere() noexcept
    {
        return 1.f / (kPi * 2.f);
    }

    /**
     * Samples a point on unit sphere
     *
     * @param u uniform sample in unit square
     */
    inline Vec3f SampleUniformSphere(Point2f u) noexcept
    {
        float z   = 1 - 2 * u[0];
        float r   = Sqrt(Max(0.f, 1 - z * z)); // avoid float error
        float phi = 2 * kPi * u[1];

        return Vec3f{r * Cos(phi), r * Sin(phi), z};
    }

    inline float PdfUniformSphere() noexcept
    {
        return 1.f / (kPi * 4.f);
    }

    /**
     * Samples a point on unit disk on xy plane
     *
     * @param u uniform sample in unit square
     */
    inline Vec3f SampleUniformDisk(Point2f u) noexcept
    {
        float r   = Sqrt(u[0]);
        float phi = 2 * kPi * u[1];

        return Vec3f{r * Cos(phi), r * Sin(phi), 0};
    }

    inline float PdfUniformDisk() noexcept
    {
        return 1.f / kPi;
    }

    //
    inline Vec3f SampleCosineWeightedHemisphere(Point2f u) noexcept
    {
        float theta = u[0] * 2 * kPi;
        float r     = Sqrt(u[1]);

        return Vec3f{Cos(theta) * r, Sin(theta) * r, Sqrt(1 - u[1])};
    }

    inline float PdfCosineWeightedHemisphere(float z) noexcept
    {
        return z / kPi;
    }

} // namespace usami
