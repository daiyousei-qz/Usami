#pragma once
#include <cstdint>
#include "usami/math/math.h"

namespace usami
{
    struct Color
    {
        union
        {
            struct
            {
                uint8_t r;
                uint8_t g;
                uint8_t b;
                uint8_t a;
            };

            uint32_t value;
        };

        constexpr Color()
        {
        }
        constexpr Color(uint32_t cc)
        {
            value = cc;
        }
        constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xff)
        {
            this->r = r;
            this->g = g;
            this->b = b;
            this->a = a;
        }
    };

    inline float GammaCorrect(float u, float gamma) noexcept
    {
        return pow(u, 1.f / gamma);
    }

    inline float ToneMap_Reinhard(float u) noexcept
    {
        constexpr float middle_gray = 1.f;

        float u_ = u * middle_gray;
        return u_ / (1.f + u_);
    }

    inline float ToneMap_Aces(float u) noexcept
    {
        constexpr float a = 2.51f;
        constexpr float b = 0.03f;
        constexpr float c = 2.43f;
        constexpr float d = 0.59f;
        constexpr float e = 0.14f;

        return (u * (a * u + b)) / (u * (c * u + d) + e);
    }

    inline float Linear2sRGB(float u) noexcept
    {
        if (u <= 0.0031308f)
        {
            return 12.92f * u;
        }
        else
        {
            float v = 1.055f * pow(u, 1.f / 2.4f) - 0.055f;
            return clamp(v, 0.f, 1.f);
        }
    }

    inline float sRGB2Linear(float u) noexcept
    {
        if (u <= 0.04045f)
        {
            return u * (1.f / 12.92f);
        }
        else
        {
            float v = pow((u + 0.055f) * (1.f / 1.055f), 2.4f);
            return clamp(v, 0.f, 1.f);
        }
    }
} // namespace usami
