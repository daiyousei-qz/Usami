#pragma once
#include "usami/ray/primitive.h"

namespace usami::ray
{
    class NaiveComposite : public IntersectableEntity
    {
    private:
        std::vector<Primitive*> objects_;

    public:
        void AddPrimitive(Primitive* body)
        {
            objects_.push_back(body);
        }

        bool Intersect(const Ray& ray, float t_min, float t_max, Workspace& ws,
                       IntersectionInfo& isect) const override
        {
            bool any_hit = false;
            float t      = t_max;

            IntersectionInfo isect_buf;
            for (auto child : objects_)
            {
                if (child->Intersect(ray, t_min, t, ws, isect_buf))
                {
                    any_hit = true;
                    t       = isect_buf.t;
                }
            }

            if (any_hit)
            {
                isect = isect_buf;
            }
            return any_hit;
        }
    };
} // namespace usami::ray
