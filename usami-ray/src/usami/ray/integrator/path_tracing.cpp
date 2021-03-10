#include "usami/ray/ray.h"
#include "usami/ray/primitive.h"
#include "usami/ray/integrator/path_tracing.h"
#include "usami/ray/light.h"
#include "usami/ray/material.h"
#include "usami/ray/bsdf.h"
#include "usami/ray/bsdf/bsdf_geometry.h"

namespace usami::ray
{
    // TODO: make unbiased
    SpectrumRGB SampleAllDirectLight(RenderingContext& ctx, Sampler& sampler, const Scene& scene,
                                     const IntersectionInfo& isect, const Vec3f& wo_bsdf,
                                     const Bsdf& bsdf, const Matrix4& world2local)
    {
        SpectrumRGB total_ld = 0.f;
        for (const Light* light : scene.Lights())
        {
            LightSample sample = light->Sample(isect, sampler.Get2D());

            if (sample.TestIllumination() && sample.TestVisibility(scene, isect, ctx.workspace))
            {
                Vec3f wi_bsdf = world2local.ApplyVector(sample.IncidentDirection());

                Vec3f incident_radiance = sample.Radiance() * AbsCosTheta(wi_bsdf);
                Vec3f exitant_radiance  = incident_radiance * bsdf.Eval(wo_bsdf, wi_bsdf);

                total_ld += exitant_radiance / sample.Pdf();
            }
        }

        return total_ld;
    }

    SpectrumRGB PathTracingIntegrator::Li(RenderingContext& ctx, Sampler& sampler,
                                          const Scene& scene, const Ray& camera_ray) const
    {
        Ray ray             = camera_ray;
        SpectrumRGB result  = 0.f;
        SpectrumRGB contrib = 1.f; // attenuation

        bool from_camera_or_specular = true;
        for (int bounce = 0; bounce < max_bounce_; ++bounce)
        {
            ctx.workspace.Clear();

            IntersectionInfo isect;
            if (!scene.Intersect(ray, ctx.workspace, isect))
            {
                // as we are not sampling from global light, we should always add this
                // if (from_camera_or_specular)
                if (scene.GlobalLight() != nullptr)
                {
                    result += contrib * scene.GlobalLight()->Eval(ray);
                }

                break;
            }

            // if ray hit an area light source
            // as we explicit sample light, the radiance from hit light source shouldn't be added
            // unless we are coming from camera or perfect specular reflection
            if (isect.area_light != nullptr && from_camera_or_specular)
            {
                result += contrib * isect.area_light->Eval(ray);
            }

            if (isect.material == nullptr)
            {
                break;
            }

            const Bsdf* bsdf = isect.material->ComputeBsdf(ctx.workspace, isect);
            USAMI_REQUIRE(bsdf != nullptr);

            Matrix4 world2local = CreateBsdfCoordTransform(isect.ns);
            Matrix4 local2world = world2local.Inverse();
            Vec3f wo_bsdf       = world2local.ApplyVector(-ray.d);

            bool is_specular_bsdf   = bsdf->GetType().Contain(BsdfType::Specular);
            from_camera_or_specular = is_specular_bsdf;

            // estimate direct light illumination for non-specular bsdf
            if (!is_specular_bsdf)
            {
                result += contrib * SampleAllDirectLight(ctx, sampler, scene, isect, wo_bsdf, *bsdf,
                                                         world2local);
            }

            // estimite indirect light illumination
            Vec3f wi_bsdf;
            float pdf_wi;
            SpectrumRGB f = bsdf->SampleAndEval(sampler.Get2D(), wo_bsdf, wi_bsdf, pdf_wi);
            if (pdf_wi == 0)
            {
                break;
            }

            contrib *= f * AbsCosTheta(wi_bsdf) / pdf_wi;
            ray = Ray{isect.point, local2world.ApplyVector(wi_bsdf)};

            // russian roulette
            if (bounce >= min_bounce_)
            {
                float prob_halt = std::max({contrib[0], contrib[1], contrib[2]});
                USAMI_REQUIRE(prob_halt <= 1);

                if (sampler.Get1D() > prob_halt)
                {
                    break;
                }

                contrib *= (1 / prob_halt);
            }
        }

        USAMI_CHECK(!InvalidSpectrum(result));
        return result;
    }
} // namespace usami::ray
