// An implementation for basic compile-time math functions
// Don't use this in runtime, performance would be slow!!!

#pragma once
#include "math_def.h"
#include <numbers>
#include <cstdint>
#include <bit>
#include <limits>
#include <concepts>

namespace usami
{
    namespace static_math
    {
        template <ScalarType T>
        constexpr inline T Abs(T x)
        {
            return x > 0 ? x : -x;
        }

        template <typename T>
        constexpr inline T Min(T x, T y)
        {
            return x < y ? x : y;
        }

        template <typename T>
        constexpr inline T Max(T x, T y)
        {
            return x < y ? y : x;
        }

        template <typename T>
        constexpr inline T Clamp(T x, T min, T max)
        {
            return Min(Max(x, min), max);
        }

        namespace detail
        {
            constexpr inline int kStaticMathDefaultMaxIteration = 20;

            constexpr inline double TruncHelper(double x)
            {
                static_assert(sizeof(double) == sizeof(uint64_t));

                uint64_t bits = std::bit_cast<uint64_t, double>(x);

                uint64_t sign     = bits >> 63;
                uint64_t exponent = bits >> 52 & 0x7ff;
                uint64_t fraction = bits & 0xfffffffffffff;

                int64_t pow = static_cast<int64_t>(exponent) - 1023;
                if (pow < 0)
                {
                    return sign ? -0. : 0.;
                }
                else if (pow < 52)
                {
                    return static_cast<double>(
                        (sign ? -1 : 1) * static_cast<int64_t>(1 << pow | fraction >> (52 - pow)));
                }
                else
                {
                    return x;
                }
            }

            constexpr inline double FloorHelper(double x)
            {
                auto t = TruncHelper(x);
                return t - (x < 0 && x != t);
            }

            constexpr inline double CeilHelper(double x)
            {
                auto t = TruncHelper(x);
                return t + (x > 0 && x != t);
            }

            constexpr inline double RoundHelper(double x)
            {
                auto r = x - FloorHelper(x);
                return FloorHelper(x) + (x > 0 && r >= 0.5) + (x < 0 && r > 0.5);
            }

            constexpr inline double ModHelper(double x, double y)
            {
                return x - FloorHelper(x / y) * y;
            }

            constexpr inline double SqrtHelper(double x, double epsilon, int max_iteration)
            {
                double term  = x;
                double delta = Max(x, 1.);
                for (int i = 0; i < max_iteration && delta > epsilon; ++i)
                {
                    double last_term = term;
                    term             = (term + x / term) / 2;
                    delta            = Abs(term - last_term);
                }

                return term;
            }

            constexpr inline double SinHelper(double x, double epsilon, int max_iteration)
            {
                x = ModHelper(x, std::numbers::pi * 2);

                double result = x;
                double term   = x;
                for (int i = 0; i < max_iteration && Abs(term) > epsilon; ++i)
                {
                    term = -term * x * x / (i * 2 + 2) / (i * 2 + 3);
                    result += term;
                }

                return result;
            }

            constexpr inline double CosHelper(double x, double epsilon, int max_iteration)
            {
                x = ModHelper(x, std::numbers::pi * 2);

                double result = 1;
                double term   = 1;
                for (int i = 0; i < max_iteration && Abs(term) > epsilon; ++i)
                {
                    term = -term * x * x / (i * 2 + 1) / (i * 2 + 2);
                    result += term;
                }

                return result;
            }

            constexpr inline double ASinHelper(double x, double epsilon, int max_iteration)
            {
                double result = x;
                double term   = x;
                for (int i = 0; i < max_iteration && Abs(term) > epsilon; ++i)
                {
                    term = term * x * x * (i * 2 + 1) / (i * 2 + 2);
                    result += term / (i * 2 + 3);
                }

                return result;
            }

            constexpr inline double ExpHelper(double x, double epsilon, int max_iteration)
            {
                double result = 1 + x;
                double term   = x;
                for (int i = 0; i < max_iteration && Abs(term) > epsilon; ++i)
                {
                    term = term * x / (i + 2);
                    result += term;
                }

                return result;
            }

            constexpr inline double LnHelper(double x, double epsilon, int max_iteration)
            {
                double term  = 1;
                double delta = std::numeric_limits<double>::max();
                for (int i = 0; i < max_iteration && delta > epsilon; ++i)
                {
                    double last_term = term;
                    double exp       = ExpHelper(term, epsilon, max_iteration);
                    term             = term + 2 * (x - exp) / (x + exp);
                    delta            = Abs(term - last_term);
                }

                return term;
            }
        } // namespace detail

        template <std::floating_point T>
        constexpr inline T Trunc(T x)
        {
            return static_cast<T>(detail::TruncHelper(static_cast<double>(x)));
        }

        template <std::floating_point T>
        constexpr inline T Floor(T x)
        {
            return static_cast<T>(detail::FloorHelper(static_cast<double>(x)));
        }

        template <std::floating_point T>
        constexpr inline T Ceil(T x)
        {
            return static_cast<T>(detail::CeilHelper(static_cast<double>(x)));
        }

        template <std::floating_point T>
        constexpr inline T Round(T x)
        {
            return static_cast<T>(detail::RoundHelper(static_cast<double>(x)));
        }

        template <std::floating_point T>
        constexpr inline T Sqrt(T x)
        {
            return static_cast<T>(detail::SqrtHelper(x, std::numeric_limits<T>::epsilon(),
                                                     detail::kStaticMathDefaultMaxIteration));
        }

        template <std::floating_point T>
        constexpr inline T Sin(T x)
        {
            return static_cast<T>(detail::SinHelper(x, std::numeric_limits<T>::epsilon(),
                                                    detail::kStaticMathDefaultMaxIteration));
        }

        template <std::floating_point T>
        constexpr inline T Cos(T x)
        {
            return static_cast<T>(detail::CosHelper(x, std::numeric_limits<T>::epsilon(),
                                                    detail::kStaticMathDefaultMaxIteration));
        }

        template <std::floating_point T>
        constexpr inline T Tan(T x)
        {
            return static_cast<T>(Sin<double>(x) / Cos<double>(x));
        }

        template <std::floating_point T>
        constexpr inline T Exp(T x)
        {
            return static_cast<T>(detail::ExpHelper(x, std::numeric_limits<T>::epsilon(),
                                                    detail::kStaticMathDefaultMaxIteration));
        }

        template <std::floating_point T>
        constexpr inline T Log(T x)
        {
            return static_cast<T>(detail::LnHelper(x, std::numeric_limits<T>::epsilon(),
                                                   detail::kStaticMathDefaultMaxIteration));
        }

        template <std::floating_point T>
        constexpr inline T Pow(T x, T y)
        {
            return static_cast<float>(Exp<double>(y * Log<double>(x)));
        }

        template <std::floating_point T>
        constexpr inline T Log2(T x)
        {
            return static_cast<T>(Log<double>(x) / std::numbers::ln2);
        }

        template <std::floating_point T>
        constexpr inline T Log10(T x)
        {
            return static_cast<T>(Log<double>(x) / std::numbers::ln10);
        }
    } // namespace static_math
} // namespace usami
