#include "usami/raster/camera.h"

namespace usami::raster
{
    static Matrix4 CreatePerspectiveTransform(float l, float r, float t, float b, float n, float f)
    {
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

    Matrix4 Camera::ComputeWorldToScreenTransform(float res_x, float res_y) const
    {
        // world-space to camera-space
        auto view_transform = Matrix4::Projection(rightward_, upward_, -lookat_, position_);
        // put projection plane at z=-1
        auto scale_transform = Matrix4::Scale(1.f / focal_len_);
        // camera-space to normalized device coordinate
        auto ky = std::tan(fov_y_ / 2) * z_near_;
        auto kx = std::tan(fov_y_ * aspect_ / 2) * z_near_;

        auto perspective_transform = CreatePerspectiveTransform(-kx, kx, ky, -ky, z_near_, z_far_);
        // normalized device coordinate to screen space
        auto screen_transform = Matrix4::Scale(-1, -1, 1)
                                    .Then(Matrix4::Translate({1, 1, 0}))
                                    .Then(Matrix4::Scale(res_x / 2, res_y / 2, 1));

        return view_transform.Then(scale_transform)
            .Then(perspective_transform)
            .Then(screen_transform);
    }
} // namespace usami::raster
