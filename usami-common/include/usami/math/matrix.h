#pragma once
#include "usami/common.h"
#include "usami/math/vector.h"

namespace usami
{
    inline constexpr Vec3f UpgradeVec(Vec2f v, float z = 1.f)
    {
        return Vec3f{v[0], v[1], z};
    }

    inline constexpr Vec4f UpgradeVec(Vec3f v, float w = 1.f)
    {
        return Vec4f{v[0], v[1], v[2], w};
    }

    inline constexpr Vec2f DowngradeVec(Vec3f v)
    {
        if (v.z == 0 || v.z == 1)
        {
            return Vec2f{v[0], v[1]};
        }
        else
        {
            return Vec2f{v[0], v[1]} / v.z;
        }
    }
    inline constexpr Vec3f DowngradeVec(Vec4f v)
    {
        if (v.w == 0 || v.w == 1)
        {
            return Vec3f{v.x, v.y, v.z};
        }
        else
        {
            return Vec3f{v.x, v.y, v.z} / v.w;
        }
    }

    inline constexpr Vec2f DowngradeVecLinear(Vec3f v)
    {
        return Vec2f{v.x, v.y};
    }
    inline constexpr Vec3f DowngradeVecLinear(Vec4f v)
    {
        return Vec3f{v.x, v.y, v.z};
    }

    class Matrix3
    {
    private:
        using VectorType = Vec3f;

        std::array<VectorType, 3> rows_;

    public:
        constexpr Matrix3()
        {
        }
        constexpr Matrix3(const VectorType& v0, const VectorType& v1, const VectorType& v2)
            : rows_{v0, v1, v2}
        {
        }
        constexpr Matrix3(std::span<const float, 9> xs)
            : rows_{VectorType(xs.subspan<0, 3>()), VectorType(xs.subspan<3, 3>()),
                    VectorType(xs.subspan<6, 3>())}
        {
        }
        constexpr Matrix3(float x00, float x01, float x02, float x10, float x11, float x12,
                          float x20, float x21, float x22)
            : rows_{VectorType(x00, x01, x02), VectorType(x10, x11, x12), VectorType(x20, x21, x22)}
        {
        }

        bool HasScaling() const noexcept
        {
            auto has_scaling_axis = [this](const Vec2f& v) {
                float delta_len = ApplyVector(v).Length() - 1;
                return delta_len > 1e-7 && delta_len < 1e-7;
            };

            return has_scaling_axis({0, 1}) && has_scaling_axis({1, 0});
        }

        constexpr VectorType& operator[](size_t index) noexcept
        {
            return rows_[index];
        }
        constexpr const VectorType& operator[](size_t index) const noexcept
        {
            return rows_[index];
        }

        constexpr Matrix3 operator*(const Matrix3& other) const noexcept
        {
            Matrix3 x = other.Transpose();

            float m[9];
            for (int i = 0; i < 3; ++i)
            {
                for (int j = 0; j < 3; ++j)
                {
                    m[i * 3 + j] = Dot(rows_[i], x.rows_[j]);
                }
            }

            return Matrix3{m};
        }

        constexpr Matrix3 Then(const Matrix3& other) const noexcept
        {
            return other * (*this);
        }

        constexpr VectorType Apply(VectorType v) const noexcept
        {
            return {Dot(v, rows_[0]), Dot(v, rows_[1]), Dot(v, rows_[2])};
        }
        constexpr Vec2f ApplyPoint(Vec2f v) const noexcept
        {
            return DowngradeVec(Apply(UpgradeVec(v, 1.f)));
        }
        constexpr Vec2f ApplyVector(Vec2f v) const noexcept
        {
            return DowngradeVecLinear(Apply(UpgradeVec(v, 0.f)));
        }

        constexpr Matrix3 Transpose() const noexcept
        {
            // clang-format off
            return Matrix3{
                rows_[0][0], rows_[1][0], rows_[2][0],
                rows_[0][1], rows_[1][1], rows_[2][1],
                rows_[0][2], rows_[1][2], rows_[2][2],
            };
            // clang-format on
        }

        constexpr Matrix3 Inverse() const noexcept
        {
            auto [a, b, c] = rows_[0].array;
            auto [d, e, f] = rows_[1].array;
            auto [g, h, l] = rows_[2].array;

            float det = a * e * l + b * f * g + c * d * h - a * f * h - b * d * l - c * e * g;
            USAMI_ASSERT(det != 0);

            float det_inv = 1.f / det;
            float inv[9];
            inv[0] = (e * l - f * h) * det_inv;
            inv[1] = (c * h - b * l) * det_inv;
            inv[2] = (b * f - c * e) * det_inv;
            inv[3] = (f * g - d * l) * det_inv;
            inv[4] = (a * l - c * g) * det_inv;
            inv[5] = (c * d - a * f) * det_inv;
            inv[6] = (d * h - e * g) * det_inv;
            inv[7] = (b * g - a * h) * det_inv;
            inv[8] = (a * e - b * d) * det_inv;

            return Matrix3{inv};
        }

        constexpr std::array<float, 9> ToArray() const noexcept
        {
            // clang-format off
            return { rows_[0][0], rows_[0][1], rows_[0][2], 
                     rows_[1][0], rows_[1][1], rows_[1][2], 
                     rows_[2][0], rows_[2][1], rows_[2][2], };
            // clang-format on
        }

    public:
        static constexpr Matrix3 Zero() noexcept
        {
            // clang-format off
            return Matrix3{
                0.f, 0.f, 0.f,
                0.f, 0.f, 0.f,
                0.f, 0.f, 0.f,
            };
            // clang-format on
        }
        static constexpr Matrix3 Identity() noexcept
        {
            // clang-format off
            return Matrix3{
                1.f, 0.f, 0.f,
                0.f, 1.f, 0.f,
                0.f, 0.f, 1.f,
            };
            // clang-format on
        }
        static constexpr Matrix3 Scale2D(float k) noexcept
        {
            // clang-format off
            return Matrix3{
                k  , 0.f, 0.f,
                0.f, k  , 0.f,
                0.f, 0.f, 1.f,
            };
            // clang-format on
        }
        static constexpr Matrix3 Scale2D(float kx, float ky) noexcept
        {
            // clang-format off
            return Matrix3{
                kx , 0.f, 0.f,
                0.f, ky , 0.f,
                0.f, 0.f, 1.f,
            };
            // clang-format on
        }
        static constexpr Matrix3 Translate2D(Vec2f v) noexcept
        {
            // clang-format off
            return Matrix3{
                1.f, 0.f, v.X(),
                0.f, 1.f, v.Y(),
                0.f, 0.f, 1.f  ,
            };
            // clang-format on
        }
        static Matrix3 Rotate2D(Vec2f pivot, float theta) noexcept
        {
            auto [x, y] = pivot.Normalize().array;
            float c     = std::cos(theta);
            float s     = std::sqrt(1 - c * c);

            // clang-format off
            return Matrix3{
                c  , -s , 0.f,
                s  , c  , 0.f,
                0.f, 0.f, 1.f,
            };
            // clang-format on
        }
        static constexpr Matrix3 ChangeBasis2D(Vec2f vx, Vec2f vy, Vec2f vp) noexcept
        {
            // clang-format off
            Matrix3 linear_projection = Matrix3{
                vx.X(), vy.X(), 0.f,
                vx.Y(), vy.Y(), 0.f,
                0.f   , 0.f   , 1.f,
            };
            // clang-format on

            return Matrix3::Translate2D(-vp).Then(linear_projection.Inverse());
        }
    };

    class Matrix4
    {
    private:
        using VectorType = Vec4f;

        std::array<VectorType, 4> rows_;

    public:
        constexpr Matrix4()
        {
        }
        constexpr Matrix4(const VectorType& v0, const VectorType& v1, const VectorType& v2,
                          const VectorType& v3)
            : rows_{v0, v1, v2, v3}
        {
        }
        constexpr Matrix4(std::span<const float, 16> xs)
            : rows_{VectorType(xs.subspan<0, 4>()), VectorType(xs.subspan<4, 4>()),
                    VectorType(xs.subspan<8, 4>()), VectorType(xs.subspan<12, 4>())}
        {
        }
        constexpr Matrix4(float x00, float x01, float x02, float x03, float x10, float x11,
                          float x12, float x13, float x20, float x21, float x22, float x23,
                          float x30, float x31, float x32, float x33)
            : rows_{VectorType(x00, x01, x02, x03), VectorType(x10, x11, x12, x13),
                    VectorType(x20, x21, x22, x23), VectorType(x30, x31, x32, x33)}
        {
        }

        bool HasScaling() const noexcept
        {
            auto has_scaling_axis = [this](const Vec3f& v) {
                float delta_len = ApplyVector(v).Length() - 1;
                return delta_len > 1e-7 && delta_len < 1e-7;
            };

            return has_scaling_axis({0, 0, 1}) && has_scaling_axis({0, 1, 0}) &&
                   has_scaling_axis({1, 0, 0});
        }

        constexpr VectorType& operator[](size_t index) noexcept
        {
            return rows_[index];
        }
        constexpr const VectorType& operator[](size_t index) const noexcept
        {
            return rows_[index];
        }

        constexpr Matrix4 operator*(const Matrix4& other) const noexcept
        {
            Matrix4 x = other.Transpose();

            float m[16];
            for (int i = 0; i < 4; ++i)
            {
                for (int j = 0; j < 4; ++j)
                {
                    m[i * 4 + j] = Dot(rows_[i], x.rows_[j]);
                }
            }

            return Matrix4{m};
        }

        constexpr Matrix4 Then(const Matrix4& other) const noexcept
        {
            return other * (*this);
        }

        constexpr VectorType Apply(VectorType v) const noexcept
        {
            return {Dot(v, rows_[0]), Dot(v, rows_[1]), Dot(v, rows_[2]), Dot(v, rows_[3])};
        }
        constexpr Vec3f ApplyPoint(Vec3f v) const noexcept
        {
            return DowngradeVec(Apply(UpgradeVec(v, 1.f)));
        }
        constexpr Vec3f ApplyVector(Vec3f v) const noexcept
        {
            return DowngradeVecLinear(Apply(UpgradeVec(v, 0.f)));
        }

        constexpr Matrix4 Transpose() const noexcept
        {
            // clang-format off
            return Matrix4{
                rows_[0][0], rows_[1][0], rows_[2][0], rows_[3][0],
                rows_[0][1], rows_[1][1], rows_[2][1], rows_[3][1],
                rows_[0][2], rows_[1][2], rows_[2][2], rows_[3][2],
                rows_[0][3], rows_[1][3], rows_[2][3], rows_[3][3],
            };
            // clang-format on
        }
        constexpr Matrix4 Inverse() const noexcept
        {
            auto [a, b, c, d] = rows_[0].array;
            auto [e, f, g, h] = rows_[1].array;
            auto [i, j, k, l] = rows_[2].array;
            auto [m, n, o, p] = rows_[3].array;

            float det;
            float inv[16];

            // clang-format off
            inv[0] = f * k * p - f * l * o - j * g * p +
                    j * h * o + n * g * l - n * h * k;

            inv[4] = -e * k * p + e * l * o + i * g * p -
                    i * h * o - m * g * l + m * h * k;

            inv[8] = e * j * p - e * l * n - i * f * p +
                    i * h * n + m * f * l - m * h * j;

            inv[12] = -e * j * o + e * k * n + i * f * o -
                    i * g * n - m * f * k + m * g * j;

            inv[1] = -b * k * p + b * l * o + j * c * p -
                    j * d * o - n * c * l + n * d * k;

            inv[5] = a * k * p - a * l * o - i * c * p +
                    i * d * o + m * c * l - m * d * k;

            inv[9] = -a * j * p + a * l * n + i * b * p -
                    i * d * n - m * b * l + m * d * j;

            inv[13] = a * j * o - a * k * n - i * b * o +
                    i * c * n + m * b * k - m * c * j;

            inv[2] = b * g * p - b * h * o - f * c * p +
                    f * d * o + n * c * h - n * d * g;

            inv[6] = -a * g * p + a * h * o + e * c * p -
                    e * d * o - m * c * h + m * d * g;

            inv[10] = a * f * p - a * h * n - e * b * p +
                    e * d * n + m * b * h - m * d * f;

            inv[14] = -a * f * o + a * g * n + e * b * o -
                    e * c * n - m * b * g + m * c * f;

            inv[3] = -b * g * l + b * h * k + f * c * l -
                    f * d * k - j * c * h + j * d * g;

            inv[7] = a * g * l - a * h * k - e * c * l +
                    e * d * k + i * c * h - i * d * g;

            inv[11] = -a * f * l + a * h * j + e * b * l -
                    e * d * j - i * b * h + i * d * f;

            inv[15] = a * f * k - a * g * j - e * b * k +
                    e * c * j + i * b * g - i * c * f;
            // clang-format on

            det = a * inv[0] + b * inv[4] + c * inv[8] + d * inv[12];
            USAMI_ASSERT(det != 0);

            float det_inv = 1.f / det;
            for (int i = 0; i < 16; ++i)
            {
                inv[i] *= det_inv;
            }

            return Matrix4{inv};
        }

        constexpr std::array<float, 16> ToArray() const noexcept
        {
            // clang-format off
            return { rows_[0][0], rows_[0][1], rows_[0][2], rows_[0][3],
                     rows_[1][0], rows_[1][1], rows_[1][2], rows_[1][3],
                     rows_[2][0], rows_[2][1], rows_[2][2], rows_[2][3],
                     rows_[3][0], rows_[3][1], rows_[3][2], rows_[3][3], };
            // clang-format on
        }

    public:
        static constexpr Matrix4 Zero() noexcept
        {
            // clang-format off
            return Matrix4{
                0.f, 0.f, 0.f, 0.f,
                0.f, 0.f, 0.f, 0.f,
                0.f, 0.f, 0.f, 0.f,
                0.f, 0.f, 0.f, 0.f,
            };
            // clang-format on
        }
        static constexpr Matrix4 Identity() noexcept
        {
            // clang-format off
            return Matrix4{
                1.f, 0.f, 0.f, 0.f,
                0.f, 1.f, 0.f, 0.f,
                0.f, 0.f, 1.f, 0.f,
                0.f, 0.f, 0.f, 1.f,
            };
            // clang-format on
        }
        static constexpr Matrix4 Scale3D(float k) noexcept
        {
            // clang-format off
            return Matrix4{
                k  , 0.f, 0.f, 0.f,
                0.f, k  , 0.f, 0.f,
                0.f, 0.f, k  , 0.f,
                0.f, 0.f, 0.f, 1.f,
            };
            // clang-format on
        }
        static constexpr Matrix4 Scale3D(float kx, float ky, float kz) noexcept
        {
            // clang-format off
            return Matrix4{
                kx , 0.f, 0.f, 0.f,
                0.f, ky , 0.f, 0.f,
                0.f, 0.f, kz , 0.f,
                0.f, 0.f, 0.f, 1.f,
            };
            // clang-format on
        }
        static constexpr Matrix4 Translate3D(Vec3f v) noexcept
        {
            // clang-format off
            return Matrix4{
                1.f, 0.f, 0.f, v.X(),
                0.f, 1.f, 0.f, v.Y(),
                0.f, 0.f, 1.f, v.Z(),
                0.f, 0.f, 0.f, 1.f  ,
            };
            // clang-format on
        }
        static Matrix4 Rotate3D(Vec3f pivot, float theta) noexcept
        {
            auto [x, y, z] = pivot.Normalize().array;
            float c        = std::cos(theta);
            float s        = std::sqrt(1 - c * c);

            // clang-format off
            return Matrix4{
                c+(1-c)*x*x  , (1-c)*x*y-s*z, (1-c)*x*z+s*y, 0,
                (1-c)*y*x+s*z, c+(1-c)*y*y  , (1-c)*y*z-s*x, 0,
                (1-c)*z*x-s*y, (1-c)*z*y+s*x, c+(1-c)*z*z  , 0,
                0            , 0            , 0            , 1,
            };
            // clang-format on
        }
        static Matrix4 RotateX3D(float theta) noexcept
        {
            float c = std::cos(theta);
            float s = std::sqrt(1 - c * c);

            // clang-format off
            return Matrix4{
                1,  0, 0, 0,
                0,  c, s, 0,
                0, -s, c, 0,
                0,  0, 0, 1,
            };
            // clang-format on
        }
        static Matrix4 RotateY3D(float theta) noexcept
        {
            float c = std::cos(theta);
            float s = std::sqrt(1 - c * c);

            // clang-format off
            return Matrix4{
                c, 0, -s, 0,
                0, 1,  0, 0,
                s, 0,  c, 0,
                0, 0,  0, 1,
            };
            // clang-format on
        }
        static Matrix4 RotateZ3D(float theta) noexcept
        {
            float c = std::cos(theta);
            float s = std::sqrt(1 - c * c);

            // clang-format off
            return Matrix4{
                c, -s, 0, 0,
                s,  c, 0, 0,
                0,  0, 1, 0,
                0,  0, 0, 1,
            };
            // clang-format on
        }
        static constexpr Matrix4 ChangeBasis3D(Vec3f vx, Vec3f vy, Vec3f vz, Vec3f vp) noexcept
        {
            // clang-format off
            Matrix4 linear_projection = Matrix4{
                vx.X(), vy.X(), vz.X(), 0,
                vx.Y(), vy.Y(), vz.Y(), 0,
                vx.Z(), vy.Z(), vz.Z(), 0,
                0     , 0     , 0     , 1,
            };
            // clang-format on

            return Matrix4::Translate3D(-vp).Then(linear_projection.Inverse());
        }
        static Matrix4 ChangeBasis3D(Vec3f vtheta, Vec3f vp) noexcept
        {
            return Matrix4::Translate3D(-vp)
                .Then(Matrix4::RotateZ3D(vtheta.Z()))
                .Then(Matrix4::RotateY3D(vtheta.Y()))
                .Then(Matrix4::RotateX3D(vtheta.X()));
        }
    }; // namespace usami
} // namespace usami
