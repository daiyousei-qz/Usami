#pragma once
#include "vector.h"
#include <cmath>
#include <algorithm>

namespace usami
{
    using std::abs;
    using std::ceil;
    using std::clamp;
    using std::cos;
    using std::floor;
    using std::max;
    using std::min;
    using std::pow;
    using std::sin;
    using std::sqrt;
    using std::tan;

    constexpr float kPi       = 3.1415926535f;
    constexpr float kInvPi    = 1.f / kPi;
    constexpr float kTwoPi    = 2.f * kPi;
    constexpr float kInvTwoPi = 1.f / kTwoPi;

    constexpr float kAreaUnitSphere     = 4.f * kPi;
    constexpr float kAreaUnitHemisphere = 2.f * kPi;

    template <typename T>
        requires std::integral<T> || std::floating_point<T> inline T Abs(T x)
    {
        return std::abs(x);
    }

    template <typename T>
    requires VecType<T> inline T Abs(T x)
    {
        return x.Abs();
    }

    template <std::floating_point Float>
    inline Float Ceil(Float x)
    {
        return std::ceil(x);
    }

    template <std::floating_point Float, size_t N>
    inline Float Dot(const Vec<Float, N>& a, const Vec<Float, N>& b)
    {
        return a.Dot(b);
    }
    template <std::floating_point Float>
    inline Vec<Float, 3> Cross(const Vec<Float, 3>& a, const Vec<Float, 3>& b)
    {
        return a.Cross(b);
    }

} // namespace usami
