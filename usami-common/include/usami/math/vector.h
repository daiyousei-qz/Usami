#pragma once
#include <array>
#include <concepts>
#include <numeric>
#include <type_traits>
#include <exception>
#include <cstdlib>
#include <cmath>
#include <smmintrin.h>

namespace usami
{
    namespace detail
    {
        inline float ExtractLowerFloat(__m128 m)
        {
            float result;
            _mm_store_ss(&result, m);
            return result;
        }

        template <typename T, size_t N, std::size_t... I>
        inline constexpr std::array<std::remove_cv_t<T>, N> ToArrayAux(const T* p,
                                                                       std::index_sequence<I...>)
        {
            return {{p[I]...}};
        }
    } // namespace detail

    template <class T, std::size_t N>
    constexpr std::array<std::remove_cv_t<T>, N> ToArray(const T* p)
    {
        return detail::ToArrayAux<T, N>(p, std::make_index_sequence<N>{});
    }

    template <typename TElem, size_t N>
    struct VecArray
    {
        using ArrayType = std::array<TElem, N>;
        std::array<TElem, N> array;

        constexpr VecArray() noexcept : array()
        {
        }

        constexpr VecArray(ArrayType data) noexcept : array(data)
        {
        }

        constexpr VecArray(TElem value) noexcept
        {
            // c++20: array.fill(value);
            for (size_t i = 0; i < N; ++i)
            {
                array[i] = value;
            }
        }

        constexpr VecArray(const TElem* p) noexcept : array(ToArray<TElem, N>(p))
        {
        }

        constexpr VecArray(std::initializer_list<float> ilist) noexcept : VecArray(ilist.begin())
        {
        }

        constexpr TElem operator[](size_t i) const noexcept
        {
            return array[i];
        }
        constexpr TElem& operator[](size_t i) noexcept
        {
            return array[i];
        }

        // additional math op
        TElem Dot(VecArray other) const noexcept
        {
            TElem ans = {};
            for (size_t i = 0; i < N; ++i)
            {
                ans += array[i] * other.array[i];
            }

            return ans;
        }
        VecArray Min(VecArray other) const noexcept
        {
            VecArray ans;
            for (size_t i = 0; i < N; ++i)
            {
                ans.array[i] = std::min(array[i], other.array[i]);
            }

            return ans;
        }
        VecArray Max(VecArray other) const noexcept
        {
            VecArray ans;
            for (size_t i = 0; i < N; ++i)
            {
                ans.array[i] = std::max(array[i], other.array[i]);
            }

            return ans;
        }
        VecArray Abs() const noexcept
        {
            VecArray ans;
            for (size_t i = 0; i < N; ++i)
            {
                ans.array[i] = std::abs(array[i]);
            }

            return ans;
        }
        VecArray Floor() const noexcept
        {
            static_assert(std::is_floating_point_v<TElem>);
            VecArray ans;
            for (size_t i = 0; i < N; ++i)
            {
                ans.array[i] = std::floor(array[i]);
            }

            return ans;
        }
        VecArray Ceil() const noexcept
        {
            static_assert(std::is_floating_point_v<TElem>);
            VecArray ans;
            for (size_t i = 0; i < N; ++i)
            {
                ans.array[i] = std::ceil(array[i]);
            }

            return ans;
        }
        VecArray Sqrt() const noexcept
        {
            static_assert(std::is_floating_point_v<TElem>);
            VecArray ans;
            for (size_t i = 0; i < N; ++i)
            {
                ans.array[i] = std::sqrt(array[i]);
            }

            return ans;
        }
        VecArray Normalize() const noexcept
        {
            static_assert(std::is_floating_point_v<TElem>);
            TElem len = std::sqrt(this->Dot(*this));
            VecArray ans;
            for (size_t i = 0; i < N; ++i)
            {
                ans.array[i] = array[i] / len;
            }
            return ans;
        }

        // unary arithmetic op
        constexpr VecArray operator+() const noexcept
        {
            return *this;
        }
        constexpr VecArray operator-() const noexcept
        {
            VecArray ans = *this;
            for (size_t i = 0; i < N; ++i)
            {
                ans[i] = -ans[i];
            }
            return ans;
        }

        // comparison
        constexpr bool operator==(VecArray other) const noexcept
        {
            return array == other.array;
        }
        constexpr bool operator!=(VecArray other) const noexcept
        {
            return array != other.array;
        }
        constexpr bool operator<(VecArray other) const noexcept
        {
            return array < other.array;
        }
        constexpr bool operator<=(VecArray other) const noexcept
        {
            return array <= other.array;
        }
        constexpr bool operator>(VecArray other) const noexcept
        {
            return array > other.array;
        }
        constexpr bool operator>=(VecArray other) const noexcept
        {
            return array >= other.array;
        }

        // elemwise arithmetic assignment
        VecArray& operator+=(VecArray other) noexcept
        {
            for (size_t i = 0; i < N; i++)
            {
                array[i] += other.array[i];
            }
            return *this;
        }
        VecArray& operator-=(VecArray other) noexcept
        {
            for (size_t i = 0; i < N; i++)
            {
                array[i] -= other.array[i];
            }
            return *this;
        }
        VecArray& operator*=(VecArray other) noexcept
        {
            for (size_t i = 0; i < N; i++)
            {
                array[i] *= other.array[i];
            }
            return *this;
        }
        VecArray& operator/=(VecArray other) noexcept
        {
            for (size_t i = 0; i < N; i++)
            {
                array[i] /= other.array[i];
            }
            return *this;
        }
        VecArray& operator%=(VecArray other) noexcept
        {
            static_assert(std::is_integral_v<TElem>);
            for (size_t i = 0; i < N; i++)
            {
                array[i] %= other.array[i];
            }
            return *this;
        }
        VecArray& operator&=(VecArray other) noexcept
        {
            static_assert(std::is_integral_v<TElem>);
            for (size_t i = 0; i < N; i++)
            {
                array[i] &= other.array[i];
            }
            return *this;
        }
        VecArray& operator|=(VecArray other) noexcept
        {
            static_assert(std::is_integral_v<TElem>);
            for (size_t i = 0; i < N; i++)
            {
                array[i] |= other.array[i];
            }
            return *this;
        }
        VecArray& operator^=(VecArray other) noexcept
        {
            static_assert(std::is_integral_v<TElem>);
            for (size_t i = 0; i < N; i++)
            {
                array[i] ^= other.array[i];
            }
            return *this;
        }
    };

    template <>
    struct VecArray<float, 3>
    {
        using ArrayType = std::array<float, 3>;
        union
        {
            __m128 xmm;
            std::array<float, 3> array;

            struct
            {
                float x, y, z;
            };
        };

        constexpr VecArray() noexcept : array()
        {
        }

        constexpr VecArray(__m128 data) noexcept : xmm(data)
        {
        }

        constexpr VecArray(ArrayType data) noexcept : array(data)
        {
        }

        constexpr VecArray(float value) noexcept : array{value, value, value}
        {
        }

        constexpr VecArray(const float* p) noexcept : array{p[0], p[1], p[2]}
        {
        }

        constexpr VecArray(std::initializer_list<float> ilist) noexcept : VecArray(ilist.begin())
        {
        }

        constexpr float operator[](size_t i) const noexcept
        {
            return array[i];
        }
        constexpr float& operator[](size_t i) noexcept
        {
            return array[i];
        }

        // additional math op
        float Dot(VecArray other) const noexcept
        {
            __m128 dp = _mm_dp_ps(xmm, other.xmm, 0x7f);
            return detail::ExtractLowerFloat(dp);
        }
        VecArray Min(VecArray other) const noexcept
        {
            return VecArray(_mm_min_ps(xmm, other.xmm));
        }
        VecArray Max(VecArray other) const noexcept
        {
            return VecArray(_mm_max_ps(xmm, other.xmm));
        }
        VecArray Abs() const noexcept
        {
            VecArray ans;
            ans.array[0] = std::abs(array[0]);
            ans.array[1] = std::abs(array[1]);
            ans.array[2] = std::abs(array[2]);
            return ans;
        }
        VecArray Floor() const noexcept
        {
            return VecArray(_mm_floor_ps(xmm));
        }
        VecArray Ceil() const noexcept
        {
            return VecArray(_mm_ceil_ps(xmm));
        }
        VecArray Sqrt() const noexcept
        {
            return VecArray{_mm_sqrt_ps(xmm)};
        }
        VecArray Cross(VecArray other) const noexcept
        {
            // auto [a1, a2, a3] = array;
            // auto [b1, b2, b3] = other.array;
            // return VecArray{ArrayType{a2 * b3 - a3 * b2, -a1 * b3 + a3 * b1, a1 * b2 - a2 * b1}};
            __m128 a = xmm;
            __m128 b = other.xmm;
            return _mm_sub_ps(_mm_mul_ps(_mm_shuffle_ps(a, a, _MM_SHUFFLE(3, 0, 2, 1)),
                                         _mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 1, 0, 2))),
                              _mm_mul_ps(_mm_shuffle_ps(a, a, _MM_SHUFFLE(3, 1, 0, 2)),
                                         _mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 0, 2, 1))));
        }
        VecArray Normalize() const noexcept
        {
            __m128 dp  = _mm_dp_ps(xmm, xmm, 0x7f);
            __m128 len = _mm_sqrt_ps(dp);
            return VecArray{_mm_div_ps(xmm, len)};
        }

        // unary arithmetic op
        VecArray operator+() const noexcept
        {
            return *this;
        }
        VecArray operator-() const noexcept
        {
            return VecArray(_mm_sub_ps(_mm_set1_ps(0.f), xmm));
        }

        // comparison
        bool operator==(VecArray other) const noexcept
        {
            __m128 cmp = _mm_cmpeq_ps(xmm, other.xmm);
            return _mm_movemask_ps(cmp) == 0x7;
        }
        bool operator!=(VecArray other) const noexcept
        {
            return !this->operator==(other);
        }
        bool operator<(VecArray other) const noexcept
        {
            return array < other.array;
        }
        bool operator<=(VecArray other) const noexcept
        {
            return array <= other.array;
        }
        bool operator>(VecArray other) const noexcept
        {
            return array > other.array;
        }
        bool operator>=(VecArray other) const noexcept
        {
            return array >= other.array;
        }

        // elemwise arithmetic assignment
        VecArray& operator+=(VecArray other) noexcept
        {
            xmm = _mm_add_ps(xmm, other.xmm);
            return *this;
        }
        VecArray& operator-=(VecArray other) noexcept
        {
            xmm = _mm_sub_ps(xmm, other.xmm);
            return *this;
        }
        VecArray& operator*=(VecArray other) noexcept
        {
            xmm = _mm_mul_ps(xmm, other.xmm);
            return *this;
        }
        VecArray& operator/=(VecArray other) noexcept
        {
            xmm = _mm_div_ps(xmm, other.xmm);
            return *this;
        }
    };

    template <>
    struct VecArray<float, 4>
    {
        using ArrayType = std::array<float, 4>;
        union
        {
            __m128 xmm;

            std::array<float, 4> array;

            struct
            {
                float x, y, z, w;
            };
        };

        constexpr VecArray() noexcept : array()
        {
        }

        constexpr VecArray(__m128 data) noexcept : xmm(data)
        {
        }

        constexpr VecArray(ArrayType data) noexcept : array(data)
        {
        }

        constexpr VecArray(float value) noexcept : array{value, value, value, value}
        {
        }

        constexpr VecArray(const float* p) noexcept : array{p[0], p[1], p[2], p[3]}
        {
        }

        constexpr VecArray(std::initializer_list<float> ilist) noexcept : VecArray(ilist.begin())
        {
        }

        constexpr float operator[](size_t i) const noexcept
        {
            return array[i];
        }
        constexpr float& operator[](size_t i) noexcept
        {
            return array[i];
        }

        // additional math op
        float Dot(VecArray other) const noexcept
        {
            __m128 dp = _mm_dp_ps(xmm, other.xmm, 0xff);
            return detail::ExtractLowerFloat(dp);
        }
        VecArray Min(VecArray other) const noexcept
        {
            return VecArray(_mm_min_ps(xmm, other.xmm));
        }
        VecArray Max(VecArray other) const noexcept
        {
            return VecArray(_mm_max_ps(xmm, other.xmm));
        }
        VecArray Abs() const noexcept
        {
            VecArray ans;
            ans.array[0] = std::abs(array[0]);
            ans.array[1] = std::abs(array[1]);
            ans.array[2] = std::abs(array[2]);
            ans.array[3] = std::abs(array[3]);
            return ans;
        }
        VecArray Floor() const noexcept
        {
            return VecArray(_mm_floor_ps(xmm));
        }
        VecArray Ceil() const noexcept
        {
            return VecArray(_mm_ceil_ps(xmm));
        }
        VecArray Sqrt() const noexcept
        {
            return VecArray{_mm_sqrt_ps(xmm)};
        }
        VecArray Normalize() const noexcept
        {
            __m128 dp  = _mm_dp_ps(xmm, xmm, 0xff);
            __m128 len = _mm_sqrt_ps(dp);
            return VecArray{_mm_div_ps(xmm, len)};
        }

        // unary arithmetic op
        VecArray operator+() const noexcept
        {
            return *this;
        }
        VecArray operator-() const noexcept
        {
            return VecArray(_mm_sub_ps(_mm_set1_ps(0.f), xmm));
        }

        // comparison
        bool operator==(VecArray other) const noexcept
        {
            __m128 cmp = _mm_cmpeq_ps(xmm, other.xmm);
            return _mm_movemask_ps(cmp) == 0xf;
        }
        bool operator!=(VecArray other) const noexcept
        {
            return !this->operator==(other);
        }
        bool operator<(VecArray other) const noexcept
        {
            return array < other.array;
        }
        bool operator<=(VecArray other) const noexcept
        {
            return array <= other.array;
        }
        bool operator>(VecArray other) const noexcept
        {
            return array > other.array;
        }
        bool operator>=(VecArray other) const noexcept
        {
            return array >= other.array;
        }

        // elemwise arithmetic assignment
        VecArray& operator+=(VecArray other) noexcept
        {
            xmm = _mm_add_ps(xmm, other.xmm);
            return *this;
        }
        VecArray& operator-=(VecArray other) noexcept
        {
            xmm = _mm_sub_ps(xmm, other.xmm);
            return *this;
        }
        VecArray& operator*=(VecArray other) noexcept
        {
            xmm = _mm_mul_ps(xmm, other.xmm);
            return *this;
        }
        VecArray& operator/=(VecArray other) noexcept
        {
            xmm = _mm_div_ps(xmm, other.xmm);
            return *this;
        }
    };

    template <typename T, size_t N>
    struct Vec;

    template <std::floating_point Float, size_t N>
    struct Vec<Float, N>
    {
        static_assert(N > 0);

        using ElementType = Float;

        static constexpr size_t VecSize = N;

        VecArray<Float, N> data;

    public:
        constexpr Vec() noexcept = default;
        constexpr Vec(Float value) noexcept : data(value)
        {
        }
        constexpr Vec(VecArray<Float, N> xs) noexcept : data(xs)
        {
        }
        constexpr Vec(std::array<Float, N> xs) : data(xs)
        {
        }
        constexpr Vec(std::initializer_list<Float> ilist) : data(ilist)
        {
            if (ilist.size() != N)
            {
                throw std::exception("incorrect number of elements in initializer list");
            }
        }

        constexpr Float& operator[](size_t index) noexcept
        {
            return data[index];
        }
        constexpr Float operator[](size_t index) const noexcept
        {
            return data[index];
        }

        constexpr std::array<Float, N>& Array() noexcept
        {
            return data.array;
        }
        constexpr const std::array<Float, N>& Array() const noexcept
        {
            return data.array;
        }

        float Length() const noexcept
        {
            return std::sqrt(data.Dot(data));
        }

#define DEFINE_FORWARDED_ITER_FUNCTION(NAME)                                                       \
    constexpr auto NAME() noexcept                                                                 \
    {                                                                                              \
        return data.array.NAME();                                                                  \
    }                                                                                              \
    constexpr auto NAME() const noexcept                                                           \
    {                                                                                              \
        return data.array.NAME();                                                                  \
    }

        DEFINE_FORWARDED_ITER_FUNCTION(begin)
        DEFINE_FORWARDED_ITER_FUNCTION(end)
        DEFINE_FORWARDED_ITER_FUNCTION(rbegin)
        DEFINE_FORWARDED_ITER_FUNCTION(rend)
#undef DEFINE_FORWARDED_ITER_FUNCTION

#define DEFINE_XYZW_ELEMENT_ACCESSOR(NAME, INDEX)                                                  \
    constexpr Float& NAME() noexcept                                                               \
    {                                                                                              \
        static_assert(N > INDEX);                                                                  \
        return this->operator[](INDEX);                                                            \
    }                                                                                              \
    constexpr Float NAME() const noexcept                                                          \
    {                                                                                              \
        static_assert(N > INDEX);                                                                  \
        return this->operator[](INDEX);                                                            \
    }

        DEFINE_XYZW_ELEMENT_ACCESSOR(X, 0)
        DEFINE_XYZW_ELEMENT_ACCESSOR(Y, 1)
        DEFINE_XYZW_ELEMENT_ACCESSOR(Z, 2)
        DEFINE_XYZW_ELEMENT_ACCESSOR(W, 3)
#undef DEFINE_XYZW_ELEMENT_ACCESSOR

        Float Dot(Vec other) const noexcept
        {
            return data.Dot(other.data);
        }
        Vec Min(Vec other) const noexcept
        {
            return data.Min(other.data);
        }
        Vec Max(Vec other) const noexcept
        {
            return data.Max(other.data);
        }
        Vec Clamp(Vec min, Vec max) const noexcept
        {
            return Max(min).Min(max);
        }
        Vec Abs() const noexcept
        {
            return data.Abs();
        }
        Vec Pow(int x) const noexcept
        {
            Vec ans = *this;
            for (int i = 0; i < N; ++i)
            {
                ans[i] = std::pow(ans[i], x);
            }
            return ans;
        }
        Vec Pow(float x) const noexcept
        {
            Vec ans = *this;
            for (int i = 0; i < N; ++i)
            {
                ans[i] = std::pow(ans[i], x);
            }
            return ans;
        }
        Vec Sin() const noexcept
        {
            Vec ans = *this;
            for (int i = 0; i < N; ++i)
            {
                ans[i] = std::sin(ans[i]);
            }
            return ans;
        }
        Vec Cos() const noexcept
        {
            Vec ans = *this;
            for (int i = 0; i < N; ++i)
            {
                ans[i] = std::cos(ans[i]);
            }
            return ans;
        }
        Vec Tan() const noexcept
        {
            Vec ans = *this;
            for (int i = 0; i < N; ++i)
            {
                ans[i] = std::tan(ans[i]);
            }
            return ans;
        }
        Vec Log() const noexcept
        {
            Vec ans = *this;
            for (int i = 0; i < N; ++i)
            {
                ans[i] = std::log(ans[i]);
            }
            return ans;
        }
        Vec Log2() const noexcept
        {
            Vec ans = *this;
            for (int i = 0; i < N; ++i)
            {
                ans[i] = std::log2(ans[i]);
            }
            return ans;
        }
        Vec Log10() const noexcept
        {
            Vec ans = *this;
            for (int i = 0; i < N; ++i)
            {
                ans[i] = std::log10(ans[i]);
            }
            return ans;
        }
        Vec Floor() const noexcept
        {
            return data.Floor();
        }
        Vec Ceil() const noexcept
        {
            return data.Ceil();
        }
        Vec Sqrt() const noexcept
        {
            return data.Sqrt();
        }
        Vec Cross(Vec other) const noexcept
        {
            return data.Cross(other.data);
        }
        Vec Normalize() const noexcept
        {
            return data.Normalize();
        }
    };

    // unary op
    //
    template <typename Float, size_t N>
    inline constexpr Vec<Float, N> operator+(Vec<Float, N> v) noexcept
    {
        return +v.data;
    }
    template <typename Float, size_t N>
    inline constexpr Vec<Float, N> operator-(Vec<Float, N> v) noexcept
    {
        return -v.data;
    }

// comparison op
//
#define DEFINE_COMPARISON_OP(OP)                                                                   \
    template <typename Float, size_t N>                                                            \
    inline constexpr bool operator OP(Vec<Float, N> lhs, Vec<Float, N> rhs) noexcept               \
    {                                                                                              \
        return lhs.data OP rhs.data;                                                               \
    }

    DEFINE_COMPARISON_OP(==)
    DEFINE_COMPARISON_OP(!=)
    DEFINE_COMPARISON_OP(<)
    DEFINE_COMPARISON_OP(<=)
    DEFINE_COMPARISON_OP(>)
    DEFINE_COMPARISON_OP(>=)
#undef DEFINE_COMPARISON_OP

// elemwise assign op
//
#define DEFINE_ELEMWISE_ASSIGN_OP(OP)                                                              \
    template <typename Float, size_t N>                                                            \
    inline constexpr Vec<Float, N>& operator OP(Vec<Float, N>& lhs, Vec<Float, N> rhs) noexcept    \
    {                                                                                              \
        lhs.data OP rhs.data;                                                                      \
        return lhs;                                                                                \
    }                                                                                              \
    template <typename Float, size_t N>                                                            \
    inline constexpr Vec<Float, N>& operator OP(Vec<Float, N>& lhs, Float rhs) noexcept            \
    {                                                                                              \
        lhs.data OP VecArray<Float, N>(rhs);                                                       \
        return lhs;                                                                                \
    }

    DEFINE_ELEMWISE_ASSIGN_OP(+=)
    DEFINE_ELEMWISE_ASSIGN_OP(-=)
    DEFINE_ELEMWISE_ASSIGN_OP(*=)
    DEFINE_ELEMWISE_ASSIGN_OP(/=)
#undef DEFINE_ELEMWISE_ASSIGN_OP

// elemwise arithmetic op
//
#define MAKE_ASSIGN_OP(OP) OP## =
#define DEFINE_ELEMWISE_ARITHMETIC_OP(OP)                                                          \
    template <typename Float, size_t N>                                                            \
    inline constexpr Vec<Float, N> operator OP(Vec<Float, N> lhs, Vec<Float, N> rhs) noexcept      \
    {                                                                                              \
        Vec<Float, N> ans = lhs;                                                                   \
        ans MAKE_ASSIGN_OP(OP) rhs;                                                                \
        return ans;                                                                                \
    }                                                                                              \
    template <typename Float, size_t N>                                                            \
    inline constexpr Vec<Float, N> operator OP(Float lhs, Vec<Float, N> rhs) noexcept              \
    {                                                                                              \
        Vec<Float, N> ans = rhs;                                                                   \
        ans MAKE_ASSIGN_OP(OP) Vec<Float, N>(lhs);                                                 \
        return ans;                                                                                \
    }                                                                                              \
    template <typename Float, size_t N>                                                            \
    inline constexpr Vec<Float, N> operator OP(Vec<Float, N> lhs, Float rhs) noexcept              \
    {                                                                                              \
        Vec<Float, N> ans = lhs;                                                                   \
        ans MAKE_ASSIGN_OP(OP) Vec<Float, N>(rhs);                                                 \
        return ans;                                                                                \
    }

    DEFINE_ELEMWISE_ARITHMETIC_OP(+)
    DEFINE_ELEMWISE_ARITHMETIC_OP(-)
    DEFINE_ELEMWISE_ARITHMETIC_OP(*)
    DEFINE_ELEMWISE_ARITHMETIC_OP(/)
#undef MAKE_ASSIGN_OP
#undef DEFINE_ELEMWISE_ARITHMETIC_OP

    template <typename T>
    struct IsVecType : public std::false_type
    {
    };

    template <typename T, size_t N>
    struct IsVecType<Vec<T, N>> : public std::true_type
    {
    };

    using Vec2f = Vec<float, 2>;
    using Vec3f = Vec<float, 3>;
    using Vec4f = Vec<float, 4>;
} // namespace usami
