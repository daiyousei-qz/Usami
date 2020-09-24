#pragma once
#include "usami/memory/arena.h"
#include "usami/color.h"
#include "usami/sampler.h"
#include "usami/ray/ray.h"
#include "usami/ray/scene.h"

namespace usami::ray
{
    struct RenderingContext
    {
        Workspace workspace;
    };

    class Integrator : public UsamiObject
    {
    public:
        virtual SpectrumRGB Li(RenderingContext& ctx, Sampler& sampler, const Scene& scene,
                               const Ray& camera_ray) const = 0;
    };
} // namespace usami::ray
