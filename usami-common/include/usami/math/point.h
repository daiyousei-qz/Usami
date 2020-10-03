#pragma once
#include <array>
#include <concepts>
#include "basic_vector.h"

namespace usami
{
    template <typename T, size_t N>
    struct Point : public detail::VecBase<T, N, false>
    {
        static_assert(N > 0);

        using BaseType = detail::VecBase<T, N, false>;

        using ArrayType   = std::array<T, N>;
        using ElementType = T;

        static constexpr VecImplConfig config = {
            .SupportEqualityComparison          = true,
            .SupportBasicArithmeticOperation    = false,
            .SupportIntegralArithmeticOperation = false,
            .SupportBasicNumericOperation       = false,
            .SupportBasicMathOperation          = false,
            .SupportDotProduct                  = false,
            .SupportCrossProduct                = false,
        };

    public:
        constexpr Point()
        {
        }
        constexpr Point(const T& x) : BaseType(x)
        {
        }
        constexpr Point(BaseType data) : BaseType(data)
        {
        }

        constexpr Point(const T& x, const T& y) requires(N == 2) : BaseType(ArrayType{x, y})
        {
        }
        constexpr Point(const T& x, const T& y, const T& z) requires(N == 3)
            : BaseType(ArrayType{x, y, z})
        {
        }
        constexpr Point(const T& x, const T& y, const T& z, const T& w) requires(N == 4)
            : BaseType(ArrayType{x, y, z, w})
        {
        }

        constexpr Point(const float* xs) : BaseType(std::span<const T, N>(xs, N))
        {
        }
        constexpr Point(std::array<T, N> xs) : BaseType(xs)
        {
        }
        constexpr Point(std::span<const T, N> xs) : BaseType(xs)
        {
        }
    };

    template <typename T>
    struct IsPointType : public std::false_type
    {
    };

    template <typename T, size_t N>
    struct IsPointType<Point<T, N>> : public std::true_type
    {
    };

    template <typename T>
    concept PointType = IsPointType<T>::value;

    using Point2i = Point<int, 2>;
    using Point3i = Point<int, 3>;

    using Point2f = Point<float, 2>;
    using Point3f = Point<float, 3>;
    using Point4f = Point<float, 4>;
} // namespace usami
