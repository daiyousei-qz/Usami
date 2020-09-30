#pragma once
#include <array>
#include <concepts>
#include <span>
#include "xsimd/xsimd.hpp"
#include "immintrin.h"

namespace usami2
{
    struct VecImplConfig
    {
        bool SupportEqualityComparison        = true;
        bool SupportLexicographicalComparison = false;

        bool SupportBasicArithmeticOperation    = true;
        bool SupportIntegralArithmeticOperation = true;

        bool SupportBitwiseOperation  = true;
        bool SupportBitShiftOperation = true;

        // abs/min/max/clamp
        bool SupportBasicNumericOperation = true;

        // float ops
        bool SupportBasicMathOperation = true;
    };

    namespace detail
    {
#define DEFINE_XYZW_ACCESSOR(NAME, INDEX)                                                          \
    constexpr T NAME() const noexcept                                                              \
    {                                                                                              \
        return array[INDEX];                                                                       \
    }

        template <typename T, size_t N>
        struct BasicVecBase
        {
            using ElementType               = T;
            static constexpr size_t VecSize = N;
        };

        template <typename T, size_t N>
        struct VecBase_Trivial : public BasicVecBase<T, N>
        {
            static constexpr bool HasSimdVec = false;
            static constexpr int SimdWidth   = 0;
            static constexpr int SimdEqMask  = 0;

            std::array<T, N> array;

            constexpr VecBase_Trivial()
            {
            }
            constexpr VecBase_Trivial(const T& x)
            {
                array.fill(x);
            }
            constexpr VecBase_Trivial(std::span<const T, N> xs)
            {
                for (size_t i = 0; i < N; ++i)
                {
                    array[i] = xs[i];
                }
            }
            constexpr VecBase_Trivial(std::array<T, N> xs) : array(xs)
            {
            }
        };

        template <typename T, size_t N>
        struct VecBase_Simd : public BasicVecBase<T, N>
        {
            static constexpr bool HasSimdVec = false;
            static constexpr int SimdWidth   = 0;
            static constexpr int SimdEqMask  = 0;

            std::array<T, N> array;

            constexpr VecBase_Simd()
            {
            }
            constexpr VecBase_Simd(const T& x)
            {
                array.fill(x);
            }
            constexpr VecBase_Simd(std::span<const T, N> xs)
            {
                for (size_t i = 0; i < N; ++i)
                {
                    array[i] = xs[i];
                }
            }
            constexpr VecBase_Simd(std::array<T, N> xs) : array(xs)
            {
            }
        };

        template <typename T>
        struct VecBase_Simd<T, 2> : public BasicVecBase<T, 2>
        {
            static constexpr bool HasSimdVec = false;
            static constexpr int SimdWidth   = 0;
            static constexpr int SimdEqMask  = 0;

            union
            {
                std::array<T, 2> array;
                struct
                {
                    T x, y;
                };
            };

            constexpr VecBase_Simd()
            {
            }
            constexpr VecBase_Simd(const T& x) : array{x, x}
            {
            }
            constexpr VecBase_Simd(std::span<const T, 2> xs) : array{xs[0], xs[1]}
            {
            }
            constexpr VecBase_Simd(std::array<T, 2> xs) : array(xs)
            {
            }

            DEFINE_XYZW_ACCESSOR(X, 0)
            DEFINE_XYZW_ACCESSOR(Y, 1)
        }; // namespace detail

        template <typename T>
        struct VecBase_Simd<T, 3> : public BasicVecBase<T, 3>
        {
            static constexpr bool HasSimdVec = true;
            static constexpr int SimdWidth   = 4;
            static constexpr int SimdEqMask  = 0b0111;

            union
            {
                std::array<T, 3> array;
                struct
                {
                    T x, y, z;
                };

                xsimd::batch<T, 4> simd_vec;
            };

            constexpr VecBase_Simd()
            {
            }
            constexpr VecBase_Simd(const T& x) : array{x, x, x}
            {
            }
            constexpr VecBase_Simd(std::span<const T, 3> xs) : array{xs[0], xs[1], xs[2]}
            {
            }
            constexpr VecBase_Simd(std::array<T, 3> xs) : array(xs)
            {
            }
            VecBase_Simd(xsimd::batch<T, 4> xs) : simd_vec(xs)
            {
            }

            DEFINE_XYZW_ACCESSOR(X, 0)
            DEFINE_XYZW_ACCESSOR(Y, 1)
            DEFINE_XYZW_ACCESSOR(Z, 2)
        };

        template <typename T>
        struct VecBase_Simd<T, 4> : public BasicVecBase<T, 4>
        {
            static constexpr bool HasSimdVec = true;
            static constexpr int SimdWidth   = 4;
            static constexpr int SimdEqMask  = 0b1111;

            union
            {
                std::array<T, 4> array;
                struct
                {
                    T x, y, z, w;
                };

                xsimd::batch<T, 4> simd_vec;
            };

            constexpr VecBase_Simd()
            {
            }
            constexpr VecBase_Simd(const T& x) : array{x, x, x, x}
            {
            }
            constexpr VecBase_Simd(std::span<const T, 4> xs) : array{xs[0], xs[1], xs[2], xs[3]}
            {
            }
            constexpr VecBase_Simd(std::array<T, 4> xs) : array(xs)
            {
            }
            VecBase_Simd(xsimd::batch<T, 4> xs) : simd_vec(xs)
            {
            }

            DEFINE_XYZW_ACCESSOR(X, 0)
            DEFINE_XYZW_ACCESSOR(Y, 1)
            DEFINE_XYZW_ACCESSOR(Z, 2)
            DEFINE_XYZW_ACCESSOR(W, 3)
        };

        template <typename T, size_t N, bool UseSimd>
        using VecBase = std::conditional_t<UseSimd, VecBase_Simd<T, N>, VecBase_Trivial<T, N>>;

        template <typename T>
        using VecElementTypeOf = typename T::ElementType;

        template <typename T>
        inline constexpr size_t VecSizeOf = T::VecSize;

        template <typename T>
        concept DerivedFromVecBase =
            std::derived_from<T, detail::BasicVecBase<VecElementTypeOf<T>, VecSizeOf<T>>>;

#undef DEFINE_XYZW_ACCESSOR
    } // namespace detail

    // a generalized vector type should have an .array member of type std::array<T, N>
    template <typename T>
    concept BasicVecType = detail::DerivedFromVecBase<T>&& requires(T x)
    {
        [] { constexpr VecImplConfig test = T::config; };
    };

    template <BasicVecType T>
    inline constexpr T operator+(const T& a) noexcept
        requires(T::config.SupportBasicArithmeticOperation)
    {
        return a;
    }
    template <BasicVecType T>
    inline constexpr T operator-(const T& a) noexcept
        requires(T::config.SupportBasicArithmeticOperation)
    {
        if (!std::is_constant_evaluated())
        {
            if constexpr (T::HasSimdVec)
            {
                return typename T::BaseType{-a.simd_vec};
            }
        }

        auto xs = a.array;
        for (int i = 0; i < T::VecSize; ++i)
        {
            xs[i] = -xs[i];
        }

        return typename T::BaseType{xs};
    }

} // namespace usami2
