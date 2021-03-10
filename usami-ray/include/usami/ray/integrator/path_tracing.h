#pragma once
#include "usami/ray/integrator.h"

namespace usami::ray
{
    class PathTracingIntegrator : public Integrator
    {
    private:
        int min_bounce_;
        int max_bounce_;

    public:
        PathTracingIntegrator(int min_bounce = 2, int max_bounce = 6)
            : min_bounce_(min_bounce), max_bounce_(max_bounce)
        {
            USAMI_REQUIRE(min_bounce > 0 && max_bounce >= min_bounce);
        }

        SpectrumRGB Li(RenderingContext& ctx, Sampler& sampler, const Scene& scene,
                       const Ray& camera_ray) const override;
    };
} // namespace usami::ray
