#pragma once
#include "vector.h"
#include "matrix.h"
#include <cmath>
#include <algorithm>

namespace usami
{
    constexpr float kPi       = 3.1415926535f;
    constexpr float kInvPi    = 1.f / kPi;
    constexpr float kTwoPi    = 2.f * kPi;
    constexpr float kInvTwoPi = 1.f / kTwoPi;

    constexpr float kAreaUnitSphere     = 4.f * kPi;
    constexpr float kAreaUnitHemisphere = 2.f * kPi;

    template <typename T>
    concept ScalarType = std::integral<T> || std::floating_point<T>;

    template <typename T>
    concept FloatScalarType = std::floating_point<T>;

    template <typename T>
    concept VecType = IsVecType<T>::value;

    template <typename T>
    concept FloatVecType = VecType<T>&& FloatScalarType<typename T::ElementType>;

    template <typename T>
    concept LinearArithmeticType = ScalarType<T> || VecType<T>;

    template <typename T>
    concept FloatLinearArithmeticType = FloatScalarType<T> || FloatVecType<T>;

    template <ScalarType T>
    inline T Abs(T x)
    {
        return std::abs(x);
    }
    template <VecType T>
    inline T Abs(T x)
    {
        return x.Abs();
    }

    template <ScalarType T>
    inline T Min(T x, T y)
    {
        return std::min(x, y);
    }
    template <VecType T>
    inline T Min(T x, T y)
    {
        return x.Min(y);
    }

    template <ScalarType T>
    inline T Max(T x, T y)
    {
        return std::max(x, y);
    }
    template <VecType T>
    inline T Max(T x, T y)
    {
        return x.Max(y);
    }

    template <ScalarType T>
    inline T Clamp(T x, T min, T max)
    {
        return std::clamp(x, min, max);
    }
    template <VecType T>
    inline T Clamp(T x, T min, T max)
    {
        return x.Clamp(min, max);
    }

    template <FloatScalarType T>
    inline T Ceil(T x)
    {
        return std::ceil(x);
    }
    template <FloatVecType T>
    inline T Ceil(T x)
    {
        return x.Ceil();
    }

    template <FloatScalarType T>
    inline T Floor(T x)
    {
        return std::floor(x);
    }
    template <FloatVecType T>
    inline T Floor(T x)
    {
        return x.Floor();
    }

    template <FloatScalarType T>
    inline T Sqrt(T x)
    {
        return std::sqrt(x);
    }
    template <FloatVecType T>
    inline T Sqrt(T x)
    {
        return x.Sqrt();
    }

    template <FloatScalarType T>
    inline T Sin(T x)
    {
        return std::sin(x);
    }
    template <FloatVecType T>
    inline T Sin(T x)
    {
        return x.Sin();
    }

    template <FloatScalarType T>
    inline T Cos(T x)
    {
        return std::cos(x);
    }
    template <FloatVecType T>
    inline T Cos(T x)
    {
        return x.Cos();
    }

    template <FloatScalarType T>
    inline T Tan(T x)
    {
        return std::tan(x);
    }
    template <FloatVecType T>
    inline T Tan(T x)
    {
        return x.Tan();
    }

    template <FloatScalarType T, ScalarType U>
    inline T Pow(T x, U y)
    {
        return std::pow(x, y);
    }
    template <FloatVecType T, ScalarType U>
    inline T Pow(T x, U y)
    {
        return x.Pow(y);
    }

    template <FloatScalarType T>
    inline T Log(T x)
    {
        return std::log10(x);
    }
    template <FloatVecType T>
    inline T Log(T x)
    {
        return x.Log10();
    }

    template <FloatScalarType T>
    inline T Log2(T x)
    {
        return std::log2(x);
    }
    template <FloatVecType T>
    inline T Log2(T x)
    {
        return x.Log2();
    }

    template <FloatScalarType T>
    inline T Log10(T x)
    {
        return std::log10(x);
    }
    template <FloatVecType T>
    inline T Log10(T x)
    {
        return x.Log10();
    }

    template <FloatLinearArithmeticType T>
    inline T Lerp(T a, T b, T t)
    {
        return a + t * (b - a);
    }

    template <FloatScalarType T, size_t N>
    inline T Dot(const Vec<T, N>& a, const Vec<T, N>& b)
    {
        return a.Dot(b);
    }
    template <FloatScalarType T>
    inline Vec<T, 3> Cross(const Vec<T, 3>& a, const Vec<T, 3>& b)
    {
        return a.Cross(b);
    }

} // namespace usami
