#pragma once
#include "usami/common.h"
#include "usami/memory/arena.h"
#include "usami/math/distribution.h"

namespace usami::ray
{
    constexpr float kTravelDistanceMin = 1e-1f;
    constexpr float kTravelDistanceMax = 1e8f;

    class Scene : public virtual UsamiObject
    {
    protected:
        DiscrateDistribution light_distribution_;

    public:
        virtual void Commit()
        {
        }

        virtual bool Intersect(const Ray& ray, Workspace& workspace,
                               IntersectionInfo& isect) const = 0;

    protected:
    };
} // namespace usami::ray
