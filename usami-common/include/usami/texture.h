#pragma once
#include "usami/math/math.h"

namespace usami
{
    enum class TextureWrapMode
    {
        Repeat,
        Clamp,
    };

    /**
     * Resolve uv coordinate into range [0, 1]
     */
    template <TextureWrapMode AddressMode = TextureWrapMode::Repeat, LinearArithmeticType T>
    inline T ResolveUV(T x) noexcept
    {
        if constexpr (AddressMode == TextureWrapMode::Repeat)
        {
            return x - Floor(x);
        }
        else if constexpr (AddressMode == TextureWrapMode::Clamp)
        {
            return Clamp(x, 0.f, 1.f);
        }
        else
        {
            static_assert(std::false_type::value);
        }
    }

    template <VecType T, size_t Dimension>
    class BasicTexture
    {
    public:
        using TexCoordType  = Vec<float, Dimension>;
        using TexSampleType = T;

        virtual ~BasicTexture() = default;

        virtual TexSampleType Eval(TexCoordType tex_coord, TexCoordType differential_x,
                                   TexCoordType differential_y) = 0;
    };

    template <typename T>
    using Texture2D = BasicTexture<T, 2>;

    template <typename T>
    using Texture3D = BasicTexture<T, 3>;
} // namespace usami
