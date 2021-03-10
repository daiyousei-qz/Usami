#pragma once
#include <numbers>
#include <concepts>

namespace usami
{
    constexpr float kFloatEpsilon = 1e-7;

    constexpr float kPi     = std::numbers::pi_v<float>;
    constexpr float kHalfPi = .5f * kPi;
    constexpr float kTwoPi  = 2.f * kPi;

    constexpr float kInvPi     = 1.f / kPi;
    constexpr float kInvHalfPi = 1.f / kHalfPi;
    constexpr float kInvTwoPi  = 1.f / kTwoPi;

    template <typename T>
    concept ScalarType = std::integral<T> || std::floating_point<T>;

    template <typename T>
    concept FloatScalarType = std::floating_point<T>;
} // namespace usami
