#pragma once
#include "usami/common.h"
#include "usami/memory/arena.h"
#include "usami/math/distribution.h"
#include "usami/ray/light.h"
#include "usami/ray/light/infinite.h"

namespace usami::ray
{
    constexpr float kTravelDistanceMin = 1e-3f;
    constexpr float kTravelDistanceMax = 1e8f;

    class Scene : public virtual UsamiObject
    {
    protected:
        const InfiniteAreaLight* global_light_;

        std::vector<const Light*> lights_;
        DiscrateDistribution light_distribution_;

    public:
        const auto& GlobalLight() const noexcept
        {
            return global_light_;
        }

        const auto& Lights() const noexcept
        {
            return lights_;
        }

        virtual void Commit()
        {
            UpdateLightDistribution();
        }

        virtual bool Intersect(const Ray& ray, Workspace& workspace,
                               IntersectionInfo& isect) const = 0;

        virtual bool IntersectQuick(const Ray& ray, Workspace& workspace,
                                    IntersectionInfo& isect) const
        {
            return Intersect(ray, workspace, isect);
        }

    protected:
        void UpdateLightDistribution()
        {
            float total_power = 0.f;
            std::vector<float> power_weights;
            for (const Light* light : lights_)
            {
                // TODO: distant/infinite light's power may overwhelm
                power_weights.push_back(light->Power().Length());
            }

            light_distribution_.Reset(power_weights.data(),
                                      power_weights.data() + power_weights.size());
        }

        void SetGlobalLightSource(const InfiniteAreaLight* light)
        {
            global_light_ = light;
        }

        void AddLightSource(const Light* light)
        {
            lights_.push_back(light);
        }
    };
} // namespace usami::ray
