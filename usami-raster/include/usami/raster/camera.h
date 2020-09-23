#pragma once
#include "usami/math/math.h"

namespace usami::raster
{
    class Camera
    {
    private:
        Vec3f position_  = {0, 0, -3};
        Vec3f lookat_    = {0, 0, 1};
        Vec3f upward_    = {0, 1, 0};
        Vec3f rightward_ = {1, 0, 0};

        float focal_len_ = 1.f;
        float fov_y_     = 3.14f / 2;
        float aspect_    = 1.f;

        float z_near_ = 1e-3f;
        float z_far_  = 1e4f;

    public:
        void SetPosition(Vec3f p)
        {
            position_ = p;
        }

        void SetDirection(Vec3f lookat, Vec3f upward)
        {
            lookat_    = lookat.Normalize();
            rightward_ = lookat.Cross(upward).Normalize();
            upward_    = rightward_.Cross(lookat_);
        }

        void SetFocalLength(float focal_len)
        {
            focal_len_ = focal_len;
        }

        void SetFieldOfView(float fov_y, float aspect)
        {
            fov_y_  = fov_y;
            aspect_ = aspect;
        }

        void SetClipRange(float z_near, float z_far)
        {
            z_near_ = z_near;
            z_far_  = z_far;
        }

        // transform world-space point into screen space
        Matrix4 ComputeWorldToScreenTransform(float res_x, float res_y) const;
    };
} // namespace usami::raster
