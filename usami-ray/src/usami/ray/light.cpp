#include "usami/ray/light.h"
#include "usami/ray/primitive.h"
#include "usami/ray/scene.h"

namespace usami::ray
{
    bool LightSample::TestVisibility(const Scene& scene, const IntersectionInfo& isect_obj,
                                     Workspace& workspace) const
    {
        IntersectionInfo isect;
        switch (type_)
        {
        case LightType::DeltaPoint:
        case LightType::Area:
        {
            if (!scene.IntersectQuick(GenerateTestRay(isect_obj.point), workspace, isect))
            {
                // no intersection with original primitive
                return false;
            }

            return isect_obj.iface == isect.iface &&
                   SamePrimitive(isect_obj.primitive, isect.primitive) &&
                   (isect.point - isect_obj.point).LengthSq() < 0.001f;
        }

        case LightType::DeltaDirection:
        case LightType::Infinite:
        {
            return !scene.IntersectQuick(GenerateShadowRay(isect_obj.point), workspace, isect);
        }

        default:
            USAMI_IMPOSSIBLE();
        }
    }
} // namespace usami::ray
