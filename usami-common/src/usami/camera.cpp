#include "usami/camera.h"

namespace usami
{
    // this matrix maps
    // (l, t, -n) -> (-1,  1, -1)
    // (r, t, -n) -> ( 1,  1, -1)
    // (l, b, -n) -> (-1, -1, -1)
    // (r, b, -n) -> ( 1, -1, -1)
    // (l', t', -f) -> (-1,  1,  1)
    // (r', t', -f) -> ( 1,  1,  1)
    // (l', b', -f) -> (-1, -1,  1)
    // (r', b', -f) -> ( 1, -1,  1)
    //
    // l' = l, r' = r, t' = t, b' = b
    static Matrix4 CreateOrthographicProjection(float l, float r, float t, float b, float n,
                                                float f)
    {
        USAMI_ASSERT(n >= 0 && f > n && l != r && t != b);
        // clang-format off
        return Matrix4{
            2/(r-l), 0      , 0      , (l+r)/(l-r),
            0      , 2/(t-b), 0      , (b+t)/(b-t),
            0      , 0      , 2/(n-f), (f+n)/(n-f),
            0      , 0      , 0      , 1          ,
        };
        // clang-format on
    }

    // this matrix maps
    // (l, t, -n) -> (-1,  1, -1)
    // (r, t, -n) -> ( 1,  1, -1)
    // (l, b, -n) -> (-1, -1, -1)
    // (r, b, -n) -> ( 1, -1, -1)
    // (l', t', -f) -> (-1,  1,  1)
    // (r', t', -f) -> ( 1,  1,  1)
    // (l', b', -f) -> (-1, -1,  1)
    // (r', b', -f) -> ( 1, -1,  1)
    //
    // l'/f = l/n, r'/f = r/n, t'/f = t/n, b'/f = b/n
    static Matrix4 CreatePerspectiveProjection(float l, float r, float t, float b, float n, float f)
    {
        USAMI_ASSERT(n >= 0 && f > n && l != r && t != b);
        // TODO: NOTE precision issue on far/near
        // clang-format off
        return Matrix4{
            2*n/(r-l), 0        , (r+l)/(r-l) , 0           ,
            0        , 2*n/(t-b), (t+b)/(t-b) , 0           ,
            0        , 0        , -(f+n)/(f-n), -2*f*n/(f-n),
            0        , 0        , -1          , 0           ,
        };
        // clang-format on
    }

    Matrix4 ComputeCameraToRasterTransform(const CameraSetting& camera, Point2i resolution,
                                           CameraProjectionType proj_type, float z_near,
                                           float z_far)
    {
        // camera-space to normalized device coordinate
        float ky = Tan(camera.fov_y / 2) * z_near;
        float kx = ky * camera.aspect;

        Matrix4 proj_transform;
        if (proj_type == CameraProjectionType::Perspective)
        {
            proj_transform = CreatePerspectiveProjection(-kx, kx, ky, -ky, z_near, z_far);
        }
        else if (proj_type == CameraProjectionType::Orthographic)
        {
            proj_transform = CreateOrthographicProjection(-kx, kx, ky, -ky, z_near, z_far);
        }

        // normalized device coordinate to raster space
        Matrix4 screen_transform =
            Matrix4::Scale3D(1, -1, 1)
                .Then(Matrix4::Translate3D({1, 1, 0}))
                .Then(Matrix4::Scale3D(resolution[0] * .5f, resolution[1] * .5f, 1));

        return proj_transform.Then(screen_transform);
    }

    Matrix4 ComputeWorldToRasterTransform(const CameraSetting& camera, Point2i resolution,
                                          CameraProjectionType proj_type, float z_near, float z_far)
    {
        return ComputeWorldToCameraTransform(camera.position,
                                             ComputeCameraOrientation(camera.lookat, camera.lookup))
            .Then(ComputeCameraToRasterTransform(camera, resolution, proj_type, z_near, z_far));
    }
} // namespace usami
