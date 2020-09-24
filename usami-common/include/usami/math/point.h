#pragma once
#include <array>
#include <concepts>

namespace usami
{
    template <typename T, size_t N>
        requires std::integral<T> || std::floating_point<T> struct Point
    {
        static_assert(N > 0);

        using ElementType = T;

        static constexpr size_t PointSize = N;

        std::array<T, N> data;

    public:
        constexpr Point() noexcept
        {
            data.fill({});
        }
        constexpr Point(T value) noexcept
        {
            data.fill(value);
        }
        constexpr Point(std::array<T, N> xs) noexcept
        {
            data = xs;
        }

        constexpr Point(T x, T y) requires(N == 2) : data{x, y}
        {
        }
        constexpr Point(T x, T y, T z) requires(N == 3) : data{x, y, z}
        {
        }
        constexpr Point(T x, T y, T z, T w) requires(N == 4) : data{x, y, z, w}
        {
        }

        constexpr T& operator[](size_t index) noexcept
        {
            return data[index];
        }
        constexpr const T& operator[](size_t index) const noexcept
        {
            return data[index];
        }

        constexpr std::array<T, N>& Array() noexcept
        {
            return data;
        }
        constexpr const std::array<T, N>& Array() const noexcept
        {
            return data;
        }

#define DEFINE_FORWARDED_ITER_FUNCTION(NAME)                                                       \
    constexpr auto NAME() noexcept                                                                 \
    {                                                                                              \
        return data.NAME();                                                                        \
    }                                                                                              \
    constexpr auto NAME() const noexcept                                                           \
    {                                                                                              \
        return data.NAME();                                                                        \
    }

        DEFINE_FORWARDED_ITER_FUNCTION(begin)
        DEFINE_FORWARDED_ITER_FUNCTION(end)
        DEFINE_FORWARDED_ITER_FUNCTION(rbegin)
        DEFINE_FORWARDED_ITER_FUNCTION(rend)
#undef DEFINE_FORWARDED_ITER_FUNCTION

#define DEFINE_XYZW_ELEMENT_ACCESSOR(NAME, INDEX)                                                  \
    constexpr T& NAME() noexcept                                                                   \
    {                                                                                              \
        static_assert(N > INDEX);                                                                  \
        return this->operator[](INDEX);                                                            \
    }                                                                                              \
    constexpr T NAME() const noexcept                                                              \
    {                                                                                              \
        static_assert(N > INDEX);                                                                  \
        return this->operator[](INDEX);                                                            \
    }

        DEFINE_XYZW_ELEMENT_ACCESSOR(X, 0)
        DEFINE_XYZW_ELEMENT_ACCESSOR(Y, 1)
        DEFINE_XYZW_ELEMENT_ACCESSOR(Z, 2)
        DEFINE_XYZW_ELEMENT_ACCESSOR(W, 3)
#undef DEFINE_XYZW_ELEMENT_ACCESSOR
    };

    template <typename T, size_t N>
    inline constexpr bool operator==(Point<T, N> lhs, Point<T, N> rhs) noexcept
    {
        return lhs.data == rhs.data;
    }
    template <typename T, size_t N>
    inline constexpr bool operator!=(Point<T, N> lhs, Point<T, N> rhs) noexcept
    {
        return lhs.data != rhs.data;
    }

    template <typename T>
    struct IsPointType : public std::false_type
    {
    };

    template <typename T, size_t N>
    struct IsPointType<Point<T, N>> : public std::true_type
    {
    };

    using Point2i = Point<int, 2>;
    using Point3i = Point<int, 3>;

    using Point2f = Point<float, 2>;
    using Point3f = Point<float, 3>;
    using Point4f = Point<float, 4>;
} // namespace usami
