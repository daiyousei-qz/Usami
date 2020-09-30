#pragma once
#include "usami/ray/material.h"
#include "usami/ray/bsdf/lambertian.h"

namespace usami::ray
{
    class DiffuseMaterial final : public Material
    {
    private:
        Vec3f albedo_;

    public:
        DiffuseMaterial(Vec3f albedo) : albedo_(albedo)
        {
        }

        const Bsdf* ComputeBsdf(Workspace& workspace, const IntersectionInfo& isect) const override
        {
            return workspace.Construct<LambertianReflection>(albedo_);
        }
    };
} // namespace usami::ray