#pragma once
#include <array>
#include <concepts>
#include <numeric>
#include <type_traits>
#include <cstdlib>
#include <cmath>
#include <span>
#include "xsimd/xsimd.hpp"
#include "immintrin.h"

namespace usami
{
    namespace detail
    {
#define DEFINE_XYZW_ACCESSOR(NAME, INDEX)                                                          \
    constexpr T NAME() const noexcept                                                              \
    {                                                                                              \
        return array[INDEX];                                                                       \
    }

        template <typename T, size_t N>
        struct VecBase_Trivial
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
        struct VecBase_Simd
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
        struct VecBase_Simd<T, 2>
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
        struct VecBase_Simd<T, 3>
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
        struct VecBase_Simd<T, 4>
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
    } // namespace detail

    template <typename T, size_t N, bool UseSimd = true>
    struct Vec : public detail::VecBase<T, N, UseSimd>
    {
        using BaseType = detail::VecBase<T, N, UseSimd>;

        using ElemType  = T;
        using ArrayType = std::array<T, N>;

        static constexpr size_t VecSize = N;

    public:
        constexpr Vec()
        {
        }
        constexpr Vec(const T& x) : BaseType(x)
        {
        }
        constexpr Vec(BaseType data) : BaseType(data)
        {
        }

        constexpr Vec(const T& x, const T& y) requires(N == 2) : BaseType(ArrayType{x, y})
        {
        }
        constexpr Vec(const T& x, const T& y, const T& z) requires(N == 3)
            : BaseType(ArrayType{x, y, z})
        {
        }
        constexpr Vec(const T& x, const T& y, const T& z, const T& w) requires(N == 4)
            : BaseType(ArrayType{x, y, z, w})
        {
        }

        constexpr Vec(const float* xs) : BaseType(std::span<const T, N>(xs, N))
        {
        }
        constexpr Vec(std::array<T, N> xs) : BaseType(xs)
        {
        }
        constexpr Vec(std::span<const T, N> xs) : BaseType(xs)
        {
        }

        constexpr T& operator[](size_t i) noexcept
        {
            return BaseType::array[i];
        }
        constexpr const T& operator[](size_t i) const noexcept
        {
            return BaseType::array[i];
        }

#define DEFINE_FORWARDED_ITER_FUNCTION(NAME)                                                       \
    auto NAME() noexcept                                                                           \
    {                                                                                              \
        return BaseType::array.NAME();                                                             \
    }                                                                                              \
    auto NAME() const noexcept                                                                     \
    {                                                                                              \
        return BaseType::array.NAME();                                                             \
    }

        DEFINE_FORWARDED_ITER_FUNCTION(begin)
        DEFINE_FORWARDED_ITER_FUNCTION(end)
        DEFINE_FORWARDED_ITER_FUNCTION(rbegin)
        DEFINE_FORWARDED_ITER_FUNCTION(rend)
#undef DEFINE_FORWARDED_ITER_FUNCTION

        constexpr ElemType LengthSq() const noexcept;
        ElemType Length() const noexcept;
        Vec Normalize() const;
    };

    template <typename T>
    struct IsVecType : std::false_type
    {
    };

    template <typename T, size_t N, bool UseSimd>
    struct IsVecType<Vec<T, N, UseSimd>> : std::true_type
    {
    };

    template <typename T>
    concept VecType = IsVecType<T>::value;

    template <VecType T>
    using VecElemType = typename T::ElemType;
    template <VecType T>
    using VecArrayType = typename T::ArrayType;

    template <VecType T>
    inline constexpr T operator+(const T& a) noexcept
    {
        return a;
    }
    template <VecType T>
    inline constexpr T operator-(const T& a) noexcept
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

    template <VecType T>
    inline bool operator==(const T& a, const T& b) noexcept
    {
        if constexpr (T::HasSimdVec)
        {
            return _mm_movemask_ps(_mm_cmpeq_ps(a.simd_vec, b.simd_vec)) == T::SimdEqMask;
        }
        else
        {
            return a.array == b.array;
        }
    }
    template <VecType T>
    inline bool operator!=(const T& a, const T& b) noexcept
    {
        return !operator==(a, b);
    }

#define MAKE_ASSIGN_OP(OP) OP## =

#define DEFINE_ELEMWISE_OP(OP)                                                                     \
    template <VecType T>                                                                           \
    inline T operator OP(const T& a, const T& b) noexcept                                          \
    {                                                                                              \
        if constexpr (T::HasSimdVec)                                                               \
        {                                                                                          \
            return typename T::BaseType{a.simd_vec OP b.simd_vec};                                 \
        }                                                                                          \
        else                                                                                       \
        {                                                                                          \
            auto xs = a.array;                                                                     \
            for (int i = 0; i < T::VecSize; ++i)                                                   \
            {                                                                                      \
                xs[i] MAKE_ASSIGN_OP(OP) b.array[i];                                               \
            }                                                                                      \
                                                                                                   \
            return typename T::BaseType{xs};                                                       \
        }                                                                                          \
    }                                                                                              \
    template <VecType T>                                                                           \
    inline T operator OP(const T& a, const VecElemType<T>& x) noexcept                             \
    {                                                                                              \
        return operator OP(a, T{x});                                                               \
    }                                                                                              \
    template <VecType T>                                                                           \
    inline T operator OP(const VecElemType<T>& x, const T& a) noexcept                             \
    {                                                                                              \
        return operator OP(a, T{x});                                                               \
    }                                                                                              \
    template <VecType T>                                                                           \
    inline T& operator MAKE_ASSIGN_OP(OP)(T& a, const T& b) noexcept                               \
    {                                                                                              \
        if constexpr (T::HasSimdVec)                                                               \
        {                                                                                          \
            a.simd_vec MAKE_ASSIGN_OP(OP) b.simd_vec;                                              \
        }                                                                                          \
        else                                                                                       \
        {                                                                                          \
            for (int i = 0; i < T::VecSize; ++i)                                                   \
            {                                                                                      \
                a.array[i] MAKE_ASSIGN_OP(OP) b.array[i];                                          \
            }                                                                                      \
        }                                                                                          \
        return a;                                                                                  \
    }                                                                                              \
    template <VecType T>                                                                           \
    inline T& operator MAKE_ASSIGN_OP(OP)(T& a, const VecElemType<T>& x) noexcept                  \
    {                                                                                              \
        return operator MAKE_ASSIGN_OP(OP)(a, T{x});                                               \
    }

    DEFINE_ELEMWISE_OP(+)
    DEFINE_ELEMWISE_OP(-)
    DEFINE_ELEMWISE_OP(*)
    DEFINE_ELEMWISE_OP(/)
    DEFINE_ELEMWISE_OP(&)
    DEFINE_ELEMWISE_OP(|)
    DEFINE_ELEMWISE_OP(^)
#undef DEFINE_ELEMWISE_OP
#undef MAKE_ASSIGN_OP

#define DEFINE_UNARY_MATH_FUNCTION(NAME, FORWARD_NAME)                                             \
    template <VecType T>                                                                           \
    inline T NAME(const T& a)                                                                      \
    {                                                                                              \
        if constexpr (T::HasSimdVec)                                                               \
        {                                                                                          \
            return typename T::BaseType{xsimd::FORWARD_NAME(a.simd_vec)};                          \
        }                                                                                          \
        else                                                                                       \
        {                                                                                          \
            auto xs = a.array;                                                                     \
            for (int i = 0; i < T::VecSize; ++i)                                                   \
            {                                                                                      \
                using namespace std;                                                               \
                using namespace xsimd;                                                             \
                xs[i] = FORWARD_NAME(a.array[i]);                                                  \
            }                                                                                      \
                                                                                                   \
            return typename T::BaseType{xs};                                                       \
        }                                                                                          \
    }

#define DEFINE_BINARY_MATH_FUNCTION(NAME, FORWARD_NAME)                                            \
    template <VecType T>                                                                           \
    inline T NAME(const T& a, const T& b)                                                          \
    {                                                                                              \
        if constexpr (T::HasSimdVec)                                                               \
        {                                                                                          \
            return typename T::BaseType{xsimd::FORWARD_NAME(a.simd_vec, b.simd_vec)};              \
        }                                                                                          \
        else                                                                                       \
        {                                                                                          \
            auto xs = a.array;                                                                     \
            for (int i = 0; i < T::VecSize; ++i)                                                   \
            {                                                                                      \
                using namespace std;                                                               \
                using namespace xsimd;                                                             \
                xs[i] = FORWARD_NAME(a.array[i], b.array[i]);                                      \
            }                                                                                      \
                                                                                                   \
            return typename T::BaseType{xs};                                                       \
        }                                                                                          \
    }                                                                                              \
    template <VecType T>                                                                           \
    inline T NAME(const T& a, const VecElemType<T>& x)                                             \
    {                                                                                              \
        return FORWARD_NAME(a, static_cast<T>(x));                                                 \
    }                                                                                              \
    template <VecType T>                                                                           \
    inline T NAME(const VecElemType<T>& x, const T& a)                                             \
    {                                                                                              \
        return FORWARD_NAME(a, static_cast<T>(x));                                                 \
    }

    DEFINE_BINARY_MATH_FUNCTION(Min, min)
    DEFINE_BINARY_MATH_FUNCTION(Max, max)

    DEFINE_UNARY_MATH_FUNCTION(Abs, abs)
    DEFINE_UNARY_MATH_FUNCTION(Asin, asin)
    DEFINE_UNARY_MATH_FUNCTION(Atan, atan)
    DEFINE_BINARY_MATH_FUNCTION(Atan2, atan2)
    DEFINE_UNARY_MATH_FUNCTION(Ceil, ceil)
    DEFINE_UNARY_MATH_FUNCTION(Cos, cos)
    DEFINE_UNARY_MATH_FUNCTION(Exp, exp)
    DEFINE_UNARY_MATH_FUNCTION(Trunc, trunc)
    DEFINE_UNARY_MATH_FUNCTION(Floor, floor)
    DEFINE_UNARY_MATH_FUNCTION(Log, log)
    DEFINE_UNARY_MATH_FUNCTION(Log10, log10)
    DEFINE_UNARY_MATH_FUNCTION(Log2, log2)
    DEFINE_UNARY_MATH_FUNCTION(Round, round)
    DEFINE_UNARY_MATH_FUNCTION(Sin, sin)
    DEFINE_UNARY_MATH_FUNCTION(Sqrt, sqrt)

#undef DEFINE_UNARY_MATH_FUNCTION
#undef DEFINE_BINARY_MATH_FUNCTION

    template <VecType T>
    T Clamp(const T& xs, const T& min, const T& max)
    {
        return Min(Max(xs, min), max);
    }
    template <VecType T>
    T Clamp(const T& xs, VecElemType<T> min, VecElemType<T> max)
    {
        return Min(Max(xs, T{min}), T{max});
    }

    template <VecType T>
    constexpr VecElemType<T> Dot(const T& a, const T& b)
    {
        if (!std::is_constant_evaluated())
        {
            if constexpr (T::HasSimdVec)
            {
                using TElem        = VecElemType<T>;
                constexpr size_t N = T::VecSize;

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

        VecElemType<T> ans = 0;
        for (int i = 0; i < T::VecSize; ++i)
        {
            ans += a.array[i] * b.array[i];
        }

        return ans;
    }

    template <VecType T>
    constexpr T Cross(const T& a, const T& b) requires(T::VecSize == 3)
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

    template <typename TElem, size_t N, bool UseSimd>
    constexpr TElem Vec<TElem, N, UseSimd>::LengthSq() const noexcept
    {
        return Dot(*this, *this);
    }
    template <typename TElem, size_t N, bool UseSimd>
    TElem Vec<TElem, N, UseSimd>::Length() const noexcept
    {
        return std::sqrt(LengthSq());
    }

    template <typename TElem, size_t N, bool UseSimd>
    Vec<TElem, N, UseSimd> Vec<TElem, N, UseSimd>::Normalize() const
    {
        auto len_helper = [&]() -> Vec<TElem, N, UseSimd> {
            if constexpr (UseSimd && std::is_same_v<TElem, float> && (N == 3 || N == 4))
            {
                constexpr int dp_mask = N == 3 ? 0x7f : 0xff;
                __m128 a_             = _mm_load_ps(this->array.data());

                Vec<TElem, N, UseSimd> result;
                _mm_store_ps(result.array.data(), _mm_dp_ps(a_, a_, dp_mask));
                return Sqrt(result);
            }
            else
            {
                return Vec<TElem, N, UseSimd>{std::sqrt(Dot(*this, *this))};
            }
        };

        return *this / len_helper();
    }

    using Vec2f = Vec<float, 2>;
    using Vec3f = Vec<float, 3>;
    using Vec4f = Vec<float, 4>;
} // namespace usami
