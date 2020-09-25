#pragma once
#include "usami/common.h"
#include "usami/ray/ray.h"
#include <type_traits>

namespace usami::ray
{
    // TODO: find a better way to express static interface
    template <typename T>
    concept GeometricShape = requires(T shape)
    {
        {
            &T::Area
        }
        ->std::same_as<float (T::*)() const noexcept>;

        {
            &T::Intersect
        }
        ->std::same_as<bool (T::*)(const Ray&, float, float, IntersectionInfo&) const noexcept>;

        {
            &T::SamplePoint
        }
        ->std::same_as<void (T::*)(const Point2f&, Vec3f&, Vec3f&, float&) const noexcept>;
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
