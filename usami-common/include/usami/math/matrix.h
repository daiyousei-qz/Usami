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
        if (v.Z() == 0 || v.Z() == 1)
        {
            return Vec2f{v[0], v[1]};
        }
        else
        {
            return Vec2f{v[0], v[1]} / v.Z();
        }
    }
    inline constexpr Vec3f DowngradeVec(Vec4f v)
    {
        if (v.W() == 0 || v.W() == 1)
        {
            return Vec3f{v[0], v[1], v[2]};
        }
        else
        {
            return Vec3f{v[0], v[1], v[2]} / v.W();
        }
    }

    inline constexpr Vec2f DowngradeVecLinear(Vec3f v)
    {
        return Vec2f{v[0], v[1]};
    }
    inline constexpr Vec3f DowngradeVecLinear(Vec4f v)
    {
        return Vec3f{v[0], v[1], v[2]};
    }

    class Matrix3
    {
    private:
        using VectorType = Vec3f;

        std::array<VectorType, 3> data;

    public:
        Matrix3()
        {
        }
        Matrix3(const float* p)
            : data{ToArray<float, 3>(p), ToArray<float, 3>(p + 3), ToArray<float, 3>(p + 6)}
        {
        }
        Matrix3(std::initializer_list<float> ilist) : Matrix3(ilist.begin())
        {
            USAMI_ASSERT(ilist.size() == 9);
        }

        VectorType& operator[](size_t index) noexcept
        {
            return data[index];
        }
        const VectorType& operator[](size_t index) const noexcept
        {
            return data[index];
        }

        Matrix3 operator*(const Matrix3& other) const noexcept
        {
            Matrix3 x = other.Transpose();

            float m[9];
            for (int i = 0; i < 3; ++i)
            {
                for (int j = 0; j < 3; ++j)
                {
                    m[i * 3 + j] = data[i].Dot(x.data[j]);
                }
            }

            return Matrix3{m};
        }

        Matrix3 Then(const Matrix3& other) const noexcept
        {
            return other * (*this);
        }

        VectorType Apply(VectorType v) const noexcept
        {
            return {v.Dot(data[0]), v.Dot(data[1]), v.Dot(data[2])};
        }

        Matrix3 Transpose() const noexcept
        {
            // clang-format off
            return Matrix3{
                data[0][0], data[1][0], data[2][0],
                data[0][1], data[1][1], data[2][1],
                data[0][2], data[1][2], data[2][2],
            };
            // clang-format on
        }

        Matrix3 Inverse() const noexcept
        {
            auto [a, b, c] = data[0].Array();
            auto [d, e, f] = data[1].Array();
            auto [g, h, l] = data[2].Array();

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

    public:
        static Matrix3 Zero() noexcept
        {
            // clang-format off
            return Matrix3{
                0.f, 0.f, 0.f,
                0.f, 0.f, 0.f,
                0.f, 0.f, 0.f,
            };
            // clang-format on
        }
        static Matrix3 Identity() noexcept
        {
            // clang-format off
            return Matrix3{
                1.f, 0.f, 0.f,
                0.f, 1.f, 0.f,
                0.f, 0.f, 1.f,
            };
            // clang-format on
        }
        static Matrix3 Scale2D(float k) noexcept
        {
            // clang-format off
            return Matrix3{
                k  , 0.f, 0.f,
                0.f, k  , 0.f,
                0.f, 0.f, 1.f,
            };
            // clang-format on
        }
        static Matrix3 Scale2D(float kx, float ky) noexcept
        {
            // clang-format off
            return Matrix3{
                kx , 0.f, 0.f,
                0.f, ky , 0.f,
                0.f, 0.f, 1.f,
            };
            // clang-format on
        }
        static Matrix3 Translate2D(Vec2f v) noexcept
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
            auto [x, y] = pivot.Normalize().Array();
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
        static Matrix3 ChangeBasis2D(Vec2f vx, Vec2f vy, Vec2f vp) noexcept
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

        std::array<VectorType, 4> data;

    public:
        Matrix4()
        {
        }
        Matrix4(const float* p)
            : data{ToArray<float, 4>(p), ToArray<float, 4>(p + 4), ToArray<float, 4>(p + 8),
                   ToArray<float, 4>(p + 12)}
        {
        }
        Matrix4(std::initializer_list<float> ilist) : Matrix4(ilist.begin())
        {
            USAMI_ASSERT(ilist.size() == 16);
        }

        VectorType& operator[](size_t index) noexcept
        {
            return data[index];
        }
        const VectorType& operator[](size_t index) const noexcept
        {
            return data[index];
        }

        Matrix4 operator*(const Matrix4& other) const noexcept
        {
            Matrix4 x = other.Transpose();

            float m[16];
            for (int i = 0; i < 4; ++i)
            {
                for (int j = 0; j < 4; ++j)
                {
                    m[i * 4 + j] = data[i].Dot(x.data[j]);
                }
            }

            return Matrix4{m};
        }

        Matrix4 Then(const Matrix4& other) const noexcept
        {
            return other * (*this);
        }

        VectorType Apply(VectorType v) const noexcept
        {
            return {v.Dot(data[0]), v.Dot(data[1]), v.Dot(data[2]), v.Dot(data[3])};
        }
        Vec3f ApplyPoint(Vec3f v) const noexcept
        {
            return DowngradeVec(Apply(UpgradeVec(v, 1.f)));
        }
        Vec3f ApplyVector(Vec3f v) const noexcept
        {
            return DowngradeVecLinear(Apply(UpgradeVec(v, 0.f)));
        }

        Matrix4 Transpose() const noexcept
        {
            // clang-format off
            return Matrix4{
                data[0][0], data[1][0], data[2][0], data[3][0],
                data[0][1], data[1][1], data[2][1], data[3][1],
                data[0][2], data[1][2], data[2][2], data[3][2],
                data[0][3], data[1][3], data[2][3], data[3][3],
            };
            // clang-format on
        }
        Matrix4 Inverse() const noexcept
        {
            auto [a, b, c, d] = data[0].Array();
            auto [e, f, g, h] = data[1].Array();
            auto [i, j, k, l] = data[2].Array();
            auto [m, n, o, p] = data[3].Array();

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

    public:
        static Matrix4 Zero() noexcept
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
        static Matrix4 Identity() noexcept
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
        static Matrix4 Scale3D(float k) noexcept
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
        static Matrix4 Scale3D(float kx, float ky, float kz) noexcept
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
        static Matrix4 Translate3D(Vec3f v) noexcept
        {
            // clang-format off
            return Matrix4{
                1.f, 0.f, 0.f, v.X(),
                0.f, 1.f, 0.f, v.Y(),
                0.f, 0.f, 1.f, v.Z(),
                0.f, 0.f, 0.f, 1.f,
            };
            // clang-format on
        }
        static Matrix4 Rotate3D(Vec3f pivot, float theta) noexcept
        {
            auto [x, y, z] = pivot.Normalize().Array();
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
        static Matrix4 ChangeBasis3D(Vec3f vx, Vec3f vy, Vec3f vz, Vec3f vp) noexcept
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
    };
} // namespace usami
