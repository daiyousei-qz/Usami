#include "usami/ray/integrator/normal_mapping.h"

namespace usami::ray
{
    SpectrumRGB NormalMappingIntegrator::Li(RenderingContext& ctx, Sampler& sampler,
                                            const Scene& scene, const Ray& camera_ray) const
    {
        ctx.workspace.Clear();

        IntersectionInfo isect;
        if (scene.Intersect(camera_ray, ctx.workspace, isect))
        {
            return Abs(isect.ns);
        }
        else
        {
            return 0.f;
        }
    }
} // namespace usami::ray
