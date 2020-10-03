#pragma once
#include <array>
#include <concepts>
#include <numeric>
#include <type_traits>
#include <cstdlib>
#include <cmath>
#include <span>
#include "basic_vector.h"
#include "xsimd/xsimd.hpp"
#include "immintrin.h"

namespace usami
{
    template <typename T, size_t N, bool UseSimd = true>
    struct Vec : public detail::VecBase<T, N, UseSimd>
    {
    public:
        using BaseType = detail::VecBase<T, N, UseSimd>;

        using ArrayType   = std::array<T, N>;
        using ElementType = T;

        static constexpr VecImplConfig config = {
            .SupportEqualityComparison          = true,
            .SupportBasicArithmeticOperation    = true,
            .SupportIntegralArithmeticOperation = std::is_integral_v<T>,
            .SupportBasicNumericOperation       = true,
            .SupportBasicMathOperation          = std::is_floating_point_v<T>,
            .SupportDotProduct                  = std::is_floating_point_v<T>,
            .SupportCrossProduct                = std::is_floating_point_v<T>,
        };

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

        constexpr ElementType LengthSq() const noexcept;
        constexpr ElementType Length() const noexcept;
        constexpr Vec Normalize() const;
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

    template <typename TElem, size_t N, bool UseSimd>
    constexpr TElem Vec<TElem, N, UseSimd>::LengthSq() const noexcept
    {
        return Dot(*this, *this);
    }
    template <typename TElem, size_t N, bool UseSimd>
    constexpr TElem Vec<TElem, N, UseSimd>::Length() const noexcept
    {
        if (std::is_constant_evaluated())
        {
            return static_math::Sqrt(LengthSq());
        }
        else
        {
            return std::sqrt(LengthSq());
        }
    }

    template <typename TElem, size_t N, bool UseSimd>
    constexpr Vec<TElem, N, UseSimd> Vec<TElem, N, UseSimd>::Normalize() const
    {
        if (std::is_constant_evaluated())
        {
            return *this / Length();
        }
        else
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
    }

    using Vec2f = Vec<float, 2>;
    using Vec3f = Vec<float, 3>;
    using Vec4f = Vec<float, 4>;
} // namespace usami
