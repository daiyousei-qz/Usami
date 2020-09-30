#pragma once
#include "point.h"
#include "vector.h"
#include "matrix.h"
#include <cmath>
#include <algorithm>

namespace usami
{
    constexpr float kFloatEpsilon = 1e-7;

    constexpr float kPi       = 3.1415926535f;
    constexpr float kInvPi    = 1.f / kPi;
    constexpr float kTwoPi    = 2.f * kPi;
    constexpr float kInvTwoPi = 1.f / kTwoPi;

    constexpr float kAreaUnitSphere     = 4.f * kPi;
    constexpr float kAreaUnitHemisphere = 2.f * kPi;

    using Array2i = std::array<int, 2>;
    using Array3i = std::array<int, 3>;
    using Array4i = std::array<int, 4>;
    using Array2f = std::array<float, 2>;
    using Array3f = std::array<float, 3>;
    using Array4f = std::array<float, 4>;

    inline constexpr float AreaUnitCone(float cos_theta) noexcept
    {
        return 2.f * kPi * (1 - cos_theta);
    }

    template <typename T>
    concept ScalarType = std::integral<T> || std::floating_point<T>;

    template <typename T>
    concept FloatScalarType = std::floating_point<T>;

    template <typename T>
    concept LinearArithmeticType = ScalarType<T> || VecType<T>;

    template <typename T>
    concept FloatLinearArithmeticType = FloatScalarType<T> || VecType<T>;

    template <ScalarType T>
    inline T Abs(T x)
    {
        return std::abs(x);
    }
    template <ScalarType T>
    inline T Min(T x, T y)
    {
        return std::min(x, y);
    }
    template <ScalarType T>
    inline T Max(T x, T y)
    {
        return std::max(x, y);
    }
    template <ScalarType T>
    inline T Clamp(T x, T min, T max)
    {
        return std::clamp(x, min, max);
    }
    template <FloatScalarType T>
    inline T Ceil(T x)
    {
        return std::ceil(x);
    }

    template <FloatScalarType T>
    inline T Floor(T x)
    {
        return std::floor(x);
    }

    template <FloatScalarType T>
    inline T Sqrt(T x)
    {
        return std::sqrt(x);
    }

    template <FloatScalarType T>
    inline T Sin(T x)
    {
        return std::sin(x);
    }

    template <FloatScalarType T>
    inline T Cos(T x)
    {
        return std::cos(x);
    }

    template <FloatScalarType T>
    inline T Tan(T x)
    {
        return std::tan(x);
    }
    template <FloatScalarType T, ScalarType U>
    inline T Pow(T x, U y)
    {
        return std::pow(x, y);
    }

    template <FloatScalarType T>
    inline T Log(T x)
    {
        return std::log10(x);
    }

    template <FloatScalarType T>
    inline T Log2(T x)
    {
        return std::log2(x);
    }

    template <FloatScalarType T>
    inline T Log10(T x)
    {
        return std::log10(x);
    }

    template <FloatLinearArithmeticType T>
    inline T Lerp(T a, T b, T t)
    {
        return a + t * (b - a);
    }
} // namespace usami
