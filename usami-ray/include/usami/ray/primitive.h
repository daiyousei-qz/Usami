#pragma once
#include "usami/common.h"
#include "usami/memory/arena.h"
#include "usami/ray/ray.h"
#include "usami/ray/bbox.h"
#include <type_traits>

namespace usami::ray
{
    // TODO: find a better way to express static interface
    template <typename T>
    concept GeometricShape = requires()
    {
        static_cast<float (T::*)() const>(&T::Area);
        static_cast<BoundingBox (T::*)() const>(&T::Bounding);
        // static_cast<bool (T::*)(const Ray&, float, float, float*, Vec3f*, Vec3f*, Vec2f*) const>(
        //     &T::IntersectTest<true>);
        // static_cast<bool (T::*)(const Ray&, float, float, float*, Vec3f*, Vec3f*, Vec2f*) const>(
        //     &T::IntersectTest<false>);
        static_cast<void (T::*)(const Point2f&, Vec3f&, Vec3f&, float&) const>(&T::SamplePoint);
    };

    template <GeometricShape ShapeType>
    inline bool TestIntersection(const ShapeType& shape, const Ray& ray, float t_min, float t_max,
                                 IntersectionInfo& isect_out)
    {
        return shape.IntersectTest<true>(ray, t_min, t_max, &isect_out.t, &isect_out.point,
                                         &isect_out.ng, &isect_out.uv);
    }

    template <GeometricShape ShapeType>
    inline bool TestOcclusion(const ShapeType& shape, const Ray& ray, float t_min, float t_max,
                              float& t_out)
    {
        return shape.IntersectTest<false>(ray, t_min, t_max, &t_out, nullptr, nullptr, nullptr);
    }

    /**
     * An `IntersectableEntity` is any geometric object of which intersection/occlusion could be
     * computed with an input ray
     */
    class IntersectableEntity : public virtual UsamiObject
    {
    public:
        /**
         * Test intersection from a given ray
         *
         * @return true if an intersection is detected, false otherwise
         */
        virtual bool Intersect(const Ray& ray, float t_min, float t_max, Workspace& ws,
                               IntersectionInfo& isect_out) const = 0;

        /**
         * Test intersection from a given ray without need for intersection info
         *
         * @return true if an intersection is detected, false otherwise
         */
        virtual bool Intersect(const Ray& ray, float t_min, float t_max, Workspace& ws,
                               OcclusionInfo& occ_out) const
        {
            IntersectionInfo isect;
            bool success = Intersect(ray, t_min, t_max, ws, isect);
            if (success)
            {
                occ_out.t         = isect.t;
                occ_out.primitive = isect.primitive;
            }

            return success;
        }
    };

    /**
     * A `Primitive` is an atomic geometric object in the scene upon which intersections are
     * computed
     */
    class Primitive : public IntersectableEntity
    {
    private:
        std::string name_;

    public:
        Primitive() : Primitive("<no-name>")
        {
        }
        Primitive(std::string name) : name_(std::move(name))
        {
        }

        const std::string& Name() const noexcept
        {
            return name_;
        }
        void SetName(std::string name) noexcept
        {
            name_ = name;
        }

        /**
         * Compute surface area of the primitive
         */
        virtual float Area() const = 0;

        /**
         * Compute surface area of the primitive
         */
        virtual BoundingBox Bounding() const = 0;

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
} // namespace usami::ray
