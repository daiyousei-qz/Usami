#pragma once
#include "usami/common.h"
#include "usami/ray/ray.h"
#include <type_traits>

namespace usami::ray
{
    template <typename T>
    concept GeometricShape = requires(T shape)
    {
        {
            shape.Area()
        }
        ->std::same_as<float>;

        {
            shape.Intersect(std::declval<Ray>(), 0.f, 0.f, std::declval<IntersectionInfo>())
        }
        ->std::same_as<bool>;

        shape.SamplePoint(std::declval<Point2f>(), std::declval<Vec3f>(), std::declval<Vec3f>(),
                          std::declval<float>());
    };

    class IntersectableEntity : public virtual UsamiObject
    {
    public:
        /**
         * Test intersection from a given ray
         *
         * @return true if an intersection is detected, false otherwise
         */
        virtual bool Intersect(const Ray& ray, float t_min, float t_max,
                               IntersectionInfo& isect) const = 0;
    };

    /**
     * A `Primitive` is an atomic geometric object in the scene upon which intersections are
     * computed
     */
    class Primitive : public IntersectableEntity
    {
    public:
        /**
         * Compute surface area of the primitive
         */
        virtual float Area() const = 0;

        /**
         * Sample a point on the primitive's surface from a unit sample
         */
        virtual void SamplePoint(const Point2f& u, Vec3f& p_out, Vec3f& n_out,
                                 float& pdf_out) const = 0;

        virtual bool Equals(const Primitive* other) const
        {
            return this == other;
        }
    };

    inline bool SamePrimitive(const Primitive* lhs, const Primitive* rhs)
    {
        USAMI_ASSERT(lhs != nullptr);
        return lhs == rhs || lhs->Equals(rhs);
    }

    class NaiveComposite : public IntersectableEntity
    {
    public:
        void AddPrimitive(Primitive* body)
        {
            objects_.push_back(body);
        }

        bool Intersect(const Ray& ray, float t_min, float t_max,
                       IntersectionInfo& isect) const override
        {
            bool any_hit = false;
            float t      = t_max;

            IntersectionInfo isect_buf;
            for (auto child : objects_)
            {
                if (child->Intersect(ray, t_min, t, isect_buf))
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

    private:
        std::vector<Primitive*> objects_;
    };
} // namespace usami::ray
