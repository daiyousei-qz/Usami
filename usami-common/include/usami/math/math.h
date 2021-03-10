#pragma once
#include "math_def.h"
#include "point.h"
#include "vector.h"
#include "matrix.h"
#include "static_math.h"
#include <cmath>
#include <algorithm>
#include <numbers>

namespace usami
{
    using Array2i = std::array<int, 2>;
    using Array3i = std::array<int, 3>;
    using Array4i = std::array<int, 4>;
    using Array2f = std::array<float, 2>;
    using Array3f = std::array<float, 3>;
    using Array4f = std::array<float, 4>;

    template <ScalarType T>
    inline T Abs(T x)
    {
        return x >= 0 ? x : -x;
    }

    template <ScalarType T>
    inline T Min(T x, T y)
    {
        return x < y ? x : y;
    }
    template <ScalarType T>
    inline T Min(std::initializer_list<T> xs)
    {
        return std::min(xs);
    }

    template <ScalarType T>
    inline T Max(T x, T y)
    {
        return x > y ? x : y;
    }
    template <ScalarType T>
    inline T Max(std::initializer_list<T> xs)
    {
        return std::max(xs);
    }

    template <ScalarType T>
    inline T Clamp(T x, T min, T max)
    {
        if (x < min)
        {
            return min;
        }
        if (x > max)
        {
            return max;
        }

        return x;
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
    inline T Ln(T x)
    {
        return std::log(x);
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

    template <FloatScalarType T>
    inline T Lerp(T a, T b, T t)
    {
        return a + t * (b - a);
    }
} // namespace usami
