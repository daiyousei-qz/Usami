#pragma once
#include <array>
#include <concepts>

namespace usami
{
    namespace detail
    {

        template <typename T, size_t N>
        struct PointBase
        {
            std::array<T, N> array;

            constexpr PointBase() : array()
            {
            }
            constexpr PointBase(std::array<T, N> xs) : array(xs)
            {
            }

            constexpr PointBase(const PointBase& other) : array(other.array)
            {
            }
            constexpr PointBase& operator=(const PointBase& other)
            {
                array = other.array;
            }
        };

        template <typename T>
        struct PointBase<T, 2>
        {
            union
            {
                std::array<T, 2> array;
                struct
                {
                    T x, y;
                };
            };

            constexpr PointBase() : array()
            {
            }
            constexpr PointBase(std::array<T, 2> xs) : array(xs)
            {
            }

            constexpr PointBase(const PointBase& other) : array(other.array)
            {
            }
            constexpr PointBase& operator=(const PointBase& other)
            {
                array = other.array;
            }
        };

        template <typename T>
        struct PointBase<T, 3>
        {
            union
            {
                std::array<T, 3> array;
                struct
                {
                    T x, y, z;
                };
            };

            constexpr PointBase() : array()
            {
            }
            constexpr PointBase(std::array<T, 3> xs) : array(xs)
            {
            }

            constexpr PointBase(const PointBase& other) : array(other.array)
            {
            }
            constexpr PointBase& operator=(const PointBase& other)
            {
                array = other.array;
            }
        };

        template <typename T>
        struct PointBase<T, 4>
        {
            union
            {
                std::array<T, 4> array;
                struct
                {
                    T x, y, z, w;
                };
            };

            constexpr PointBase() : array()
            {
            }
            constexpr PointBase(std::array<T, 4> xs) : array(xs)
            {
            }

            constexpr PointBase(const PointBase& other) : array(other.array)
            {
            }
            constexpr PointBase& operator=(const PointBase& other)
            {
                array = other.array;
            }
        };
    } // namespace detail

    template <typename T, size_t N>
        requires std::integral<T> || std::floating_point<T> struct Point
        : public detail::PointBase<T, N>
    {
        static_assert(N > 0);

        using BaseType = detail::PointBase<T, N>;
        using ElemType = T;

        static constexpr size_t PointSize = N;

    public:
        constexpr Point() noexcept
        {
        }
        constexpr Point(T value) noexcept
        {
            BaseType::array.fill(value);
        }
        constexpr Point(const T* p) noexcept
        {
            for (int i = 0; i < N; ++i)
            {
                BaseType::array[i] = p[i];
            }
        }
        constexpr Point(std::array<T, N> xs) noexcept : BaseType(xs)
        {
        }

        constexpr Point(T x, T y) requires(N == 2)
        {
            BaseType::array[0] = x;
            BaseType::array[1] = y;
        }
        constexpr Point(T x, T y, T z) requires(N == 3)
        {
            BaseType::array[0] = x;
            BaseType::array[1] = y;
            BaseType::array[2] = z;
        }
        constexpr Point(T x, T y, T z, T w) requires(N == 4)
        {
            BaseType::array[0] = x;
            BaseType::array[1] = y;
            BaseType::array[2] = z;
            BaseType::array[3] = w;
        }

        constexpr T& operator[](size_t index) noexcept
        {
            return BaseType::array[index];
        }
        constexpr const T& operator[](size_t index) const noexcept
        {
            return BaseType::array[index];
        }

#define DEFINE_FORWARDED_ITER_FUNCTION(NAME)                                                       \
    constexpr auto NAME() noexcept                                                                 \
    {                                                                                              \
        return BaseType::array.NAME();                                                             \
    }                                                                                              \
    constexpr auto NAME() const noexcept                                                           \
    {                                                                                              \
        return BaseType::array.NAME();                                                             \
    }

        DEFINE_FORWARDED_ITER_FUNCTION(begin)
        DEFINE_FORWARDED_ITER_FUNCTION(end)
        DEFINE_FORWARDED_ITER_FUNCTION(rbegin)
        DEFINE_FORWARDED_ITER_FUNCTION(rend)
#undef DEFINE_FORWARDED_ITER_FUNCTION
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

    template <typename T, size_t N>
    inline constexpr bool operator==(Point<T, N> lhs, Point<T, N> rhs) noexcept
    {
        return lhs.array == rhs.array;
    }
    template <typename T, size_t N>
    inline constexpr bool operator!=(Point<T, N> lhs, Point<T, N> rhs) noexcept
    {
        return lhs.array != rhs.array;
    }

    using Point2i = Point<int, 2>;
    using Point3i = Point<int, 3>;

    using Point2f = Point<float, 2>;
    using Point3f = Point<float, 3>;
    using Point4f = Point<float, 4>;
} // namespace usami
