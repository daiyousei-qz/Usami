#pragma once
#include "usami/ray/scene.h"

#include "usami/ray/primitive.h"
#include "usami/ray/primitive/geometric.h"
#include "usami/ray/composite/bvh.h"

#include "usami/ray/light.h"
#include "usami/ray/light/point.h"
#include "usami/ray/light/diffuse.h"
#include "usami/ray/light/distant.h"
#include "usami/ray/light/infinite.h"
#include "usami/ray/light/spot.h"

namespace usami::ray
{
    class IntegratedScene : public Scene
    {
    private:
        Arena arena_;

        std::vector<Primitive*> prims_;

        IntersectableEntity* world_;

    public:
        void Commit() override
        {
            auto world = arena_.Construct<BvhComposite>(prims_);
            // auto world = arena_.Construct<NaiveComposite>();
            // for (auto prim : prims_)
            // {
            //     world->AddPrimitive(prim);
            // }

            world_ = world;
        }

        bool Intersect(const Ray& ray, Workspace& workspace,
                       IntersectionInfo& isect_out) const override
        {
            return world_->Intersect(ray, kTravelDistanceMin, kTravelDistanceMax, isect_out);
        }

        // primitive factory
        //
        template <typename ShapeType>
        void AddGeometricPrimitive(ShapeType shape, shared_ptr<Material> mat,
                                   bool reverse_orientation = false)
        {
            auto object =
                arena_.Construct<GeometricPrimitive<ShapeType>>(shape, reverse_orientation);
            object->BindMaterial(move(mat));

            prims_.push_back(object);
        }
        template <GeometricShape ShapeType>
        void AddGeometricLight(ShapeType shape, SpectrumRGB intensity, bool reverse_orientation)
        {
            auto object =
                arena_.Construct<GeometricPrimitive<ShapeType>>(shape, reverse_orientation);
            object->BindAreaLight<DiffuseAreaLight>(intensity);
            AddLightSource(object->GetAreaLight());

            prims_.push_back(object);
        }

        void AddPointLight(Vec3f point, SpectrumRGB intensity)
        {
            AddLightSource(arena_.Construct<PointLight>(point, intensity));
        }
        void AddSpotLight(Vec3f point, Vec3f direction, float theta, SpectrumRGB intensity)
        {
            AddLightSource(arena_.Construct<SpotLight>(point, direction, theta, intensity));
        }

        void AddDistantLight(const Vec3f& direction, const SpectrumRGB& color, Vec3f world_center,
                             float world_radius)
        {
            AddLightSource(
                arena_.Construct<DistantLight>(direction, color, world_center, world_radius));
        }

        void AddInfiniteAreaLight(shared_ptr<Texture2D<Vec3f>> tex, float intensity,
                                  Vec3f world_center, float world_radius)
        {
            SetGlobalLightSource(arena_.Construct<InfiniteAreaLight>(std::move(tex), intensity,
                                                                     world_center, world_radius));
        }
    };
} // namespace usami::ray
