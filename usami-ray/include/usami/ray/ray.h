#pragma once
#include "usami/math/math.h"
#include <concepts>

namespace usami::ray
{
    class AreaLight;
    class Material;
    class Primitive;

    struct Ray
    {
        Vec3f o; // origin
        Vec3f d; // direction

        // create a ray from src point to dest point
        static Ray FromTo(Vec3f src, Vec3f dest) noexcept
        {
            return Ray{src, (dest - src).Normalize()};
        }
    };

    struct OcclusionInfo final
    {
        // distance that ray travels to make the hit
        float t;

        // object that the ray hits
        const Primitive* primitive;
    };

    struct IntersectionInfo final
    {
        // Basic Info
        //

        // distance that ray travels to make the hit
        float t;

        // Geometric Info
        //

        // point where intersection happens
        Vec3f point;

        // normal vector of geometric surface at the hit point
        Vec3f ng;

        // uv coordianate at the hit point for texture mapping
        Vec2f uv = {0.f, 0.f};

        // index of polygon face hit in a mesh
        unsigned iface = 0;

        // Render Info
        //

        // normal vector of scattering surface at the hit point
        Vec3f ns;

        // object that the ray hits
        const Primitive* primitive = nullptr;

        // material at the hit surface
        const Material* material = nullptr;

        // area light instance at the hit surface, if any
        const AreaLight* area_light = nullptr;
    };

    template <typename T>
    concept IntersectionTestOutputType =
        std::same_as<T, OcclusionInfo> || std::same_as<T, IntersectionInfo>;
} // namespace usami::ray
