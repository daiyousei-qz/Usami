#pragma once
#include "usami/math/math.h"
#include "usami/camera.h"
#include "usami/ray/ray.h"

namespace usami::ray
{
    class PerspectiveCamera
    {
    private:
        CameraSetting setting_;
        Point2i resolution_;

        Matrix4 raster_to_world_;

    public:
        PerspectiveCamera(const CameraSetting& setting, Point2i resolution)
            : setting_(setting), resolution_(resolution)
        {
            raster_to_world_ = ComputeWorldToRasterTransform(
                                   setting, resolution, CameraProjectionType::Perspective, 1, 2)
                                   .Inverse();
        }

        Ray SpawnRay(Point2i screen_pos, Point2f camera_sample) const noexcept
        {
            float x = static_cast<float>(screen_pos.x) + camera_sample.x - .5f;
            float y = static_cast<float>(screen_pos.y) + camera_sample.y - .5f;

            Vec3f ray_dir = raster_to_world_.ApplyPoint(Vec3f{x, y, 0}) - setting_.position;
            return Ray{setting_.position, ray_dir.Normalize()};
        }
    };
} // namespace usami::ray
