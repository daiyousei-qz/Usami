#pragma once
#include "usami/ray/integrator.h"

namespace usami::ray
{
    class NormalMappingIntegrator : public Integrator
    {
    public:
        NormalMappingIntegrator()
        {
        }

        SpectrumRGB Li(RenderingContext& ctx, Sampler& sampler, const Scene& scene,
                       const Ray& camera_ray) const override;
    };
} // namespace usami::ray
