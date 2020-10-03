#pragma once
#include <array>
#include <concepts>
#include <span>
#include <cstdint>
#include "static_math.h"
#include "xsimd/xsimd.hpp"
#include "immintrin.h"

namespace usami
{
    struct VecImplConfig
    {
        bool SupportEqualityComparison = true;
        // bool SupportLexicographicalComparison = false;

        bool SupportBasicArithmeticOperation    = true;
        bool SupportIntegralArithmeticOperation = true;

        // abs/min/max/clamp
        bool SupportBasicNumericOperation = true;

        // float ops
        bool SupportBasicMathOperation = true;
        bool SupportDotProduct         = true;
        bool SupportCrossProduct       = true;
    };

    namespace detail
    {

#define DEFINE_XYZW_ACCESSOR(NAME, INDEX)                                                          \
    constexpr T NAME() const noexcept                                                              \
    {                                                                                              \
        return this->array[INDEX];                                                                 \
    }

        template <typename T, size_t N>
        struct CommonVecBase
        {
            using ElementType               = T;
            using ArrayType                 = std::array<T, N>;
            static constexpr size_t VecSize = N;

            constexpr CommonVecBase()
            {
            }
        };

        template <typename T, size_t N>
        struct VecBase_Trivial : public CommonVecBase<T, N>
        {
            using SimdVecType = void*;

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

        template <typename T>
        struct VecBase_Trivial<T, 2> : public CommonVecBase<T, 2>
        {
            using SimdVecType = void*;

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

            constexpr VecBase_Trivial()
            {
            }
            constexpr VecBase_Trivial(const T& x) : array{x, x}
            {
            }
            constexpr VecBase_Trivial(std::span<const T, 2> xs) : array{xs[0], xs[1]}
            {
            }
            constexpr VecBase_Trivial(std::array<T, 2> xs) : array(xs)
            {
            }

            DEFINE_XYZW_ACCESSOR(X, 0)
            DEFINE_XYZW_ACCESSOR(Y, 1)
        }; // namespace detail

        template <typename T>
        struct VecBase_Trivial<T, 3> : public CommonVecBase<T, 3>
        {
            using SimdVecType = void*;

            static constexpr bool HasSimdVec = false;
            static constexpr int SimdWidth   = 0;
            static constexpr int SimdEqMask  = 0;

            union
            {
                std::array<T, 3> array;
                struct
                {
                    T x, y, z;
                };
            };

            constexpr VecBase_Trivial()
            {
            }
            constexpr VecBase_Trivial(const T& x) : array{x, x, x}
            {
            }
            constexpr VecBase_Trivial(std::span<const T, 3> xs) : array{xs[0], xs[1], xs[2]}
            {
            }
            constexpr VecBase_Trivial(std::array<T, 3> xs) : array(xs)
            {
            }

            DEFINE_XYZW_ACCESSOR(X, 0)
            DEFINE_XYZW_ACCESSOR(Y, 1)
            DEFINE_XYZW_ACCESSOR(Z, 2)
        };

        template <typename T>
        struct VecBase_Trivial<T, 4> : public CommonVecBase<T, 4>
        {
            using SimdVecType = void*;

            static constexpr bool HasSimdVec = false;
            static constexpr int SimdWidth   = 0;
            static constexpr int SimdEqMask  = 0;

            union
            {
                std::array<T, 4> array;
                struct
                {
                    T x, y, z, w;
                };
            };

            constexpr VecBase_Trivial()
            {
            }
            constexpr VecBase_Trivial(const T& x) : array{x, x, x, x}
            {
            }
            constexpr VecBase_Trivial(std::span<const T, 4> xs) : array{xs[0], xs[1], xs[2], xs[3]}
            {
            }
            constexpr VecBase_Trivial(std::array<T, 4> xs) : array(xs)
            {
            }

            DEFINE_XYZW_ACCESSOR(X, 0)
            DEFINE_XYZW_ACCESSOR(Y, 1)
            DEFINE_XYZW_ACCESSOR(Z, 2)
            DEFINE_XYZW_ACCESSOR(W, 3)
        };

        template <typename T, size_t N>
        struct VecBase_Simd : public CommonVecBase<T, N>
        {
            using SimdVecType = void*;

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
        struct VecBase_Simd<T, 2> : public CommonVecBase<T, 2>
        {
            using SimdVecType = void*;

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
        struct VecBase_Simd<T, 3> : public CommonVecBase<T, 3>
        {
            using SimdVecType = xsimd::batch<T, 4>;

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
        struct VecBase_Simd<T, 4> : public CommonVecBase<T, 4>
        {
            using SimdVecType = xsimd::batch<T, 4>;

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
#undef DEFINE_XYZW_ACCESSOR

        template <typename T, size_t N, bool UseSimd>
        using ConditionalVecBase =
            std::conditional_t<UseSimd, VecBase_Simd<T, N>, VecBase_Trivial<T, N>>;

        template <typename T, size_t N, bool UseSimd>
        struct VecBase : public ConditionalVecBase<T, N, UseSimd>
        {
            using ImplBaseType = ConditionalVecBase<T, N, UseSimd>;
            using SimdVecType  = typename ImplBaseType::SimdVecType;

            using BaseType = VecBase;

            constexpr VecBase()
            {
            }
            constexpr VecBase(const T& x) : ImplBaseType(x)
            {
            }
            constexpr VecBase(std::span<const T, N> xs) : ImplBaseType(xs)
            {
            }
            constexpr VecBase(std::array<T, N> xs) : ImplBaseType(xs)
            {
            }
            VecBase(SimdVecType xs) requires(UseSimd) : ImplBaseType(xs)
            {
            }

            std::array<T, N>& Array() noexcept
            {
                return this->array;
            }
            const std::array<T, N>& Array() const noexcept
            {
                return this->array;
            }

            constexpr T& operator[](size_t i) noexcept
            {
                return this->array[i];
            }
            constexpr const T& operator[](size_t i) const noexcept
            {
                return this->array[i];
            }

#define DEFINE_FORWARDED_ITER_FUNCTION(NAME)                                                       \
    auto NAME() noexcept                                                                           \
    {                                                                                              \
        return this->array.NAME();                                                                 \
    }                                                                                              \
    auto NAME() const noexcept                                                                     \
    {                                                                                              \
        return this->array.NAME();                                                                 \
    }
            DEFINE_FORWARDED_ITER_FUNCTION(begin)
            DEFINE_FORWARDED_ITER_FUNCTION(end)
            DEFINE_FORWARDED_ITER_FUNCTION(rbegin)
            DEFINE_FORWARDED_ITER_FUNCTION(rend)
#undef DEFINE_FORWARDED_ITER_FUNCTION
        };

        template <typename T>
        using VecBaseTypeOf = typename T::BaseType;

        template <typename T>
        using VecElementTypeOf = typename T::ElementType;

        template <typename T>
        using VecArrayTypeOf = typename T::ArrayType;

        template <typename T>
        inline constexpr size_t VecSizeOf = T::VecSize;

        template <typename T>
        concept DerivedFromVecBase =
            std::derived_from<T, detail::CommonVecBase<VecElementTypeOf<T>, VecSizeOf<T>>>;
    } // namespace detail

    // a generalized vector type should have an .array member of type std::array<T, N>
    template <typename T>
    concept BasicVecType = requires(T x)
    {
        // derive from the corresponding VecBase
        detail::DerivedFromVecBase<T>;

        // could be constructed from VecBase
        T{std::declval<detail::VecBaseTypeOf<T>>()};

        // has a config for supported ops
        {
            T::config
        }
        ->std::convertible_to<VecImplConfig>;
        // TODO: due to a MSVC bug, the following constraint doesn't work now
        // [] { constexpr VecImplConfig test = T::config; };
    };

    template <BasicVecType T>
    inline constexpr bool operator==(const T& a, const T& b) noexcept
        requires(T::config.SupportEqualityComparison)
    {
        if (!std::is_constant_evaluated())
        {
            if constexpr (T::HasSimdVec)
            {
                // TODO: not assuming float
                return _mm_movemask_ps(_mm_cmpeq_ps(a.simd_vec, b.simd_vec)) == T::SimdEqMask;
            }
        }

        return a.array == b.array;
    }
    template <BasicVecType T>
    inline constexpr bool operator!=(const T& a, const T& b) noexcept
        requires(T::config.SupportEqualityComparison)
    {
        return !operator==(a, b);
    }

    namespace detail
    {
        template <BasicVecType T, typename F>
        constexpr T VecForwardUnaryOp(const T& a, F f)
        {
            detail::VecArrayTypeOf<T> xs;
            for (int i = 0; i < detail::VecSizeOf<T>; ++i)
            {
                xs[i] = f(a.array[i]);
            }

            return detail::VecBaseTypeOf<T>{xs};
        }

        template <BasicVecType T, typename F>
        constexpr T VecForwardBinaryOp(const T& a, const T& b, F f)
        {
            detail::VecArrayTypeOf<T> xs;
            for (int i = 0; i < detail::VecSizeOf<T>; ++i)
            {
                xs[i] = f(a.array[i], b.array[i]);
            }

            return detail::VecBaseTypeOf<T>{xs};
        }
    } // namespace detail

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
                return detail::VecBaseTypeOf<T>{-a.simd_vec};
            }
        }

        return detail::VecForwardUnaryOp(
            a, [](auto x) constexpr { return -x; });
    }

#define MAKE_ASSIGN_OP(OP) OP## =
#define DEFINE_ELEMWISE_OP(OP, TOGGLE)                                                             \
    template <BasicVecType T>                                                                      \
    inline constexpr T operator OP(const T& a, const T& b) noexcept requires(T::config.TOGGLE)     \
    {                                                                                              \
        if (!std::is_constant_evaluated())                                                         \
        {                                                                                          \
            if constexpr (T::HasSimdVec)                                                           \
            {                                                                                      \
                return detail::VecBaseTypeOf<T>{a.simd_vec OP b.simd_vec};                         \
            }                                                                                      \
        }                                                                                          \
                                                                                                   \
        return detail::VecForwardBinaryOp(                                                         \
            a, b, [](auto x, auto y) constexpr { return x OP y; });                                \
    }                                                                                              \
    template <BasicVecType T>                                                                      \
    inline constexpr T operator OP(                                                                \
        const T& a, const detail::VecElementTypeOf<T>& x) noexcept requires(T::config.TOGGLE)      \
    {                                                                                              \
        return operator OP(a, T{x});                                                               \
    }                                                                                              \
    template <BasicVecType T>                                                                      \
    inline constexpr T operator OP(const detail::VecElementTypeOf<T>& x,                           \
                                   const T& a) noexcept requires(T::config.TOGGLE)                 \
    {                                                                                              \
        return operator OP(T{x}, a);                                                               \
    }                                                                                              \
    template <BasicVecType T>                                                                      \
    inline constexpr T& operator MAKE_ASSIGN_OP(OP)(T& a, const T& b) noexcept requires(           \
        T::config.TOGGLE)                                                                          \
    {                                                                                              \
        a = a OP b;                                                                                \
        return a;                                                                                  \
    }                                                                                              \
    template <BasicVecType T>                                                                      \
    inline constexpr T& operator MAKE_ASSIGN_OP(OP)(                                               \
        T& a, const detail::VecElementTypeOf<T>& x) noexcept requires(T::config.TOGGLE)            \
    {                                                                                              \
        return operator MAKE_ASSIGN_OP(OP)(a, T{x});                                               \
    }

    DEFINE_ELEMWISE_OP(+, SupportBasicArithmeticOperation)
    DEFINE_ELEMWISE_OP(-, SupportBasicArithmeticOperation)
    DEFINE_ELEMWISE_OP(*, SupportBasicArithmeticOperation)
    DEFINE_ELEMWISE_OP(/, SupportBasicArithmeticOperation)
    DEFINE_ELEMWISE_OP(%, SupportIntegralArithmeticOperation)
    DEFINE_ELEMWISE_OP(&, SupportIntegralArithmeticOperation)
    DEFINE_ELEMWISE_OP(|, SupportIntegralArithmeticOperation)
    DEFINE_ELEMWISE_OP(^, SupportIntegralArithmeticOperation)
#undef DEFINE_ELEMWISE_OP
#undef MAKE_ASSIGN_OP

#define DEFINE_UNARY_MATH_FUNCTION(NAME, FORWARD_NAME, TOGGLE)                                     \
    template <BasicVecType T>                                                                      \
    inline constexpr T NAME(const T& a) requires(T::config.TOGGLE)                                 \
    {                                                                                              \
        if (std::is_constant_evaluated())                                                          \
        {                                                                                          \
            return detail::VecForwardUnaryOp(                                                      \
                a, [](auto x) constexpr { return static_math::NAME(x); });                         \
        }                                                                                          \
        else                                                                                       \
        {                                                                                          \
            if constexpr (T::HasSimdVec)                                                           \
            {                                                                                      \
                return detail::VecBaseTypeOf<T>{xsimd::FORWARD_NAME(a.simd_vec)};                  \
            }                                                                                      \
            else                                                                                   \
            {                                                                                      \
                using namespace std;                                                               \
                using namespace xsimd;                                                             \
                return detail::VecForwardUnaryOp(a, [](auto x) { return FORWARD_NAME(x); });       \
            }                                                                                      \
        }                                                                                          \
    }

#define DEFINE_BINARY_MATH_FUNCTION(NAME, FORWARD_NAME, TOGGLE)                                    \
    template <BasicVecType T>                                                                      \
    inline constexpr T NAME(const T& a, const T& b) requires(T::config.TOGGLE)                     \
    {                                                                                              \
        if (std::is_constant_evaluated())                                                          \
        {                                                                                          \
            return detail::VecForwardBinaryOp(                                                     \
                a, b, [](auto x, auto y) constexpr { return static_math::NAME(x, y); });           \
        }                                                                                          \
        else                                                                                       \
        {                                                                                          \
            if constexpr (T::HasSimdVec)                                                           \
            {                                                                                      \
                return detail::VecBaseTypeOf<T>{xsimd::FORWARD_NAME(a.simd_vec, b.simd_vec)};      \
            }                                                                                      \
            else                                                                                   \
            {                                                                                      \
                using namespace std;                                                               \
                using namespace xsimd;                                                             \
                return detail::VecForwardBinaryOp(                                                 \
                    a, b, [](auto x, auto y) { return FORWARD_NAME(x, y); });                      \
            }                                                                                      \
        }                                                                                          \
    }                                                                                              \
    template <BasicVecType T>                                                                      \
    inline constexpr T NAME(const T& a,                                                            \
                            const detail::VecElementTypeOf<T>& x) requires(T::config.TOGGLE)       \
    {                                                                                              \
        return FORWARD_NAME(a, T{x});                                                              \
    }                                                                                              \
    template <BasicVecType T>                                                                      \
    inline constexpr T NAME(const detail::VecElementTypeOf<T>& x,                                  \
                            const T& a) requires(T::config.TOGGLE)                                 \
    {                                                                                              \
        return FORWARD_NAME(T{x}, a);                                                              \
    }

    DEFINE_UNARY_MATH_FUNCTION(Abs, abs, SupportBasicNumericOperation)
    DEFINE_BINARY_MATH_FUNCTION(Min, min, SupportBasicNumericOperation)
    DEFINE_BINARY_MATH_FUNCTION(Max, max, SupportBasicNumericOperation)

    // DEFINE_UNARY_MATH_FUNCTION(Asin, asin)
    // DEFINE_UNARY_MATH_FUNCTION(Atan, atan)
    // DEFINE_BINARY_MATH_FUNCTION(Atan2, atan2)
    DEFINE_UNARY_MATH_FUNCTION(Ceil, ceil, SupportBasicMathOperation)
    DEFINE_UNARY_MATH_FUNCTION(Cos, cos, SupportBasicMathOperation)
    DEFINE_UNARY_MATH_FUNCTION(Exp, exp, SupportBasicMathOperation)
    DEFINE_UNARY_MATH_FUNCTION(Trunc, trunc, SupportBasicMathOperation)
    DEFINE_UNARY_MATH_FUNCTION(Floor, floor, SupportBasicMathOperation)
    DEFINE_UNARY_MATH_FUNCTION(Log, log, SupportBasicMathOperation)
    DEFINE_UNARY_MATH_FUNCTION(Log10, log10, SupportBasicMathOperation)
    DEFINE_UNARY_MATH_FUNCTION(Log2, log2, SupportBasicMathOperation)
    DEFINE_UNARY_MATH_FUNCTION(Round, round, SupportBasicMathOperation)
    DEFINE_UNARY_MATH_FUNCTION(Sin, sin, SupportBasicMathOperation)
    DEFINE_UNARY_MATH_FUNCTION(Sqrt, sqrt, SupportBasicMathOperation)

#undef DEFINE_UNARY_MATH_FUNCTION
#undef DEFINE_BINARY_MATH_FUNCTION

    template <BasicVecType T>
    constexpr T Clamp(const T& xs, const T& min,
                      const T& max) requires(T::config.SupportBasicNumericOperation)
    {
        return Min(Max(xs, min), max);
    }
    template <BasicVecType T>
    constexpr T
    Clamp(const T& xs, detail::VecElementTypeOf<T> min,
          detail::VecElementTypeOf<T> max) requires(T::config.SupportBasicNumericOperation)
    {
        return Min(Max(xs, T{min}), T{max});
    }

    template <BasicVecType T>
    constexpr detail::VecElementTypeOf<T> Dot(const T& a,
                                              const T& b) requires(T::config.SupportDotProduct)
    {
        using TElem        = detail::VecElementTypeOf<T>;
        constexpr size_t N = detail::VecSizeOf<T>;

        if (!std::is_constant_evaluated())
        {
            if constexpr (T::HasSimdVec)
            {
                if constexpr (std::is_same_v<TElem, float> && (N == 3 || N == 4))
                {
                    constexpr int dp_mask = N == 3 ? 0x71 : 0xf1;
                    return _mm_cvtss_f32(_mm_dp_ps(a.simd_vec, b.simd_vec, dp_mask));
                }
                if constexpr (N == 3)
                {
                    auto a_simd_vec = a.simd_vec;
                    a_simd_vec[3]   = 0;

                    return xsimd::hadd(a_simd_vec * b.simd_vec);
                }
                else
                {
                    return xsimd::hadd(a.simd_vec * b.simd_vec);
                }
            }
        }

        TElem ans = 0;
        for (int i = 0; i < N; ++i)
        {
            ans += a.array[i] * b.array[i];
        }

        return ans;
    }

    template <BasicVecType T>
    constexpr T Cross(const T& a,
                      const T& b) requires(T::VecSize == 3 && T::config.SupportCrossProduct)
    {
        if (!std::is_constant_evaluated())
        {
            if constexpr (T::HasSimdVec)
            {
                static_assert(T::SimdWidth == 4);

                auto ans = _mm_sub_ps(_mm_mul_ps(_mm_permute_ps(a.simd_vec, _MM_PERM_DACB),
                                                 _mm_permute_ps(b.simd_vec, _MM_PERM_DBAC)),
                                      _mm_mul_ps(_mm_permute_ps(a.simd_vec, _MM_PERM_DBAC),
                                                 _mm_permute_ps(b.simd_vec, _MM_PERM_DACB)));

                return typename T::BaseType{ans};
            }
        }

        const auto& [a1, a2, a3] = a.array;
        const auto& [b1, b2, b3] = b.array;

        return T{a2 * b3 - a3 * b2, -a1 * b3 + a3 * b1, a1 * b2 - a2 * b1};
    }

} // namespace usami
