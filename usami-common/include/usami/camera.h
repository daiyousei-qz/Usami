#pragma once
#include "usami/math/math.h"

namespace usami
{
    enum class CameraProjectionType
    {
        Perspective,
        Orthographic,
    };

    struct CameraSetting
    {
        // position of the camera
        Vec3f position = {0, 0, 0};

        // camera looking position
        Vec3f lookat = {0, 0, 1};

        // camera upward position
        Vec3f lookup = {0, 1, 0};

        // y-axis field of view, in radian
        float fov_y = kPi * 0.5f;

        // aspect ratio, fov_x = fov_y * aspect
        float aspect = 1;
    };

    struct CameraOrientation
    {
        Vec3f forward;
        Vec3f upward;
        Vec3f rightward;
    };

    inline CameraOrientation ComputeCameraOrientation(Vec3f lookat, Vec3f lookup)
    {
        CameraOrientation result;
        result.forward   = lookat.Normalize();
        result.rightward = Cross(result.forward, lookup).Normalize();
        result.upward    = Cross(result.rightward, result.forward);

        return result;
    }

    Matrix4 ComputeWorldToScreenTransform(const CameraSetting& camera, Point2i resolution,
                                          CameraProjectionType proj_type, float z_near,
                                          float z_far);
} // namespace usami
