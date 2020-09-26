#pragma once
#include "usami/math/math.h"

namespace usami::ray
{
    // calculation of Bsdf instance would be done in local coordinate
    // where scattering normal vector is mapped to kBsdfNormal
    constexpr Vec3f kBsdfNormal = Vec3f{0.f, 0.f, 1.f};

    // create transform from **world coordinate** to **bsdf local coordinate**
    // TODO: this transform only support non-polarized bsdf
    // TODO: generate inverse matrix, too
    inline Matrix4 CreateBsdfCoordTransform(Vec3f n)
    {
        if (n.x != 0 || n.y != 0)
        {
            Vec3f nz = n.Normalize();
            Vec3f nx = Vec3f{n.y, -n.x, 0.f}.Normalize();
            Vec3f ny = Cross(n, nx);

            return Matrix4::ChangeBasis3D(nx, ny, nz, 0.f);
        }
        else
        {
            Vec3f nz = n.Normalize();
            Vec3f nx = Vec3f{n.z > 0 ? 1.f : -1.f, 0.f, 0.f};
            Vec3f ny = Vec3f{0.f, 1.f, 0.f};

            return Matrix4::ChangeBasis3D(nx, ny, nz, 0.f);
        }
    }

    inline bool SameHemisphere(const Vec3f& wo, const Vec3f& wi) noexcept
    {
        return wo.z * wi.z > 0;
    }

    inline float CosTheta(const Vec3f& w) noexcept
    {
        return w.z;
    }
    inline float Cos2Theta(const Vec3f& w) noexcept
    {
        return w.z * w.z;
    }
    inline float AbsCosTheta(const Vec3f& w) noexcept
    {
        return Abs(w.z);
    }
    inline float Sin2Theta(const Vec3f& w) noexcept
    {
        return Max(0.f, 1.f - Cos2Theta(w));
    }

    inline float SinTheta(const Vec3f& w) noexcept
    {
        return Sqrt(Sin2Theta(w));
    }

    inline float TanTheta(const Vec3f& w) noexcept
    {
        return SinTheta(w) / CosTheta(w);
    }

    inline float Tan2Theta(const Vec3f& w) noexcept
    {
        return Sin2Theta(w) / Cos2Theta(w);
    }

    // assume SameHemiSphere(wo, n)
    inline Vec3f ReflectRay(const Vec3f& wo, const Vec3f& n) noexcept
    {
        auto h = Dot(wo, n);
        return -wo + 2 * h * n;
    }

    // assume n = +-kBsdfNormal
    inline Vec3f ReflectRayQuick(const Vec3f& wo) noexcept
    {
        return Vec3f{-wo.x, -wo.y, wo.z};
    }

    // assume SameHemiSphere(wo, n)
    // eta = etaI / etaT, i.e. inverse of refractive index
    inline bool RefractRay(const Vec3f& wo, const Vec3f& n, float eta, Vec3f& refracted) noexcept
    {
        float cosThetaI  = Dot(n, wo);
        float sin2ThetaI = Max(0.f, 1.f - cosThetaI * cosThetaI);
        float sin2ThetaT = eta * eta * sin2ThetaI;
        if (sin2ThetaT > 1)
        {
            return false;
        }

        float cosThetaT = Sqrt(1.f - sin2ThetaT);

        refracted = -eta * wo + (eta * cosThetaI - cosThetaT) * n;
        return true;
    }

    // eta = etaI / etaT
    inline constexpr float Schlick(float cos_theta, float eta) noexcept
    {
        auto r0 = (eta - 1) / (eta + 1);
        r0      = r0 * r0;

        auto root  = 1 - cos_theta;
        auto root2 = root * root;
        return r0 + (1 - r0) * root2 * root;
    }
} // namespace usami::ray
