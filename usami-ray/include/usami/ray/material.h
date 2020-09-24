#pragma once
#include "usami/common.h"
#include "usami/memory/arena.h"
#include "usami/ray/bsdf.h"
#include "usami/ray/ray.h"

namespace usami::ray
{
    class Material : public UsamiObject
    {
    public:
        /**
         * Compute BSDF at the intersection point of the surface
         *
         * Note `Bsdf` object is allocated from `workspace` provided.
         */
        virtual const Bsdf* ComputeBsdf(Workspace& workspace,
                                        const IntersectionInfo& isect) const = 0;
    };
} // namespace usami::ray
