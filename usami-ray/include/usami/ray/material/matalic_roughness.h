#pragma once
#include "usami/ray/material.h"
#include "usami/color.h"
#include "usami/texture.h"

namespace usami::ray
{
    class MatalicRoughnessMaterial final : public Material
    {
    private:
        // Vec3f emmisive_factor_;
        // Texture2D<Vec3f>* emissive_tex_;

        Vec3f base_color_factor_;
        Texture2D<Vec3f>* base_color_tex_;

        float matallic_factor;
        float roughness_factor;

    public:
        MatalicRoughnessMaterial(/* Vec3f emissive_factor, Texture2D<Vec3f>* emissive_tex, */
                                 Vec3f base_color, Texture2D<Vec3f>* base_color_tex, float matallic,
                                 float roughness)
        {
        }

        const Bsdf* ComputeBsdf(Workspace& workspace, const IntersectionInfo& isect) const override
        {
            USAMI_NO_IMPL();
        }
    };
} // namespace usami::ray
