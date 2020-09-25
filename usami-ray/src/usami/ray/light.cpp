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
            if (!scene.Intersect(GenerateTestRay(isect_obj.point), workspace, isect))
            {
                // no intersection with original primitive
                return false;
            }

            return SamePrimitive(isect_obj.object, isect.object) &&
                   (isect.point - isect_obj.point).LengthSq() < 0.001f;
        }

        case LightType::DeltaDirection:
        case LightType::Infinite:
        {
            return !scene.Intersect(GenerateShadowRay(isect_obj.point), workspace, isect);
        }

        default:
            USAMI_IMPOSSIBLE();
        }
    }
} // namespace usami::ray
