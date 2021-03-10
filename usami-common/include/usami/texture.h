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
    template <typename T, TextureWrapMode AddressMode = TextureWrapMode::Repeat>
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
        using TexCoordType        = Vec<float, Dimension>;
        using TexSampleType       = T;
        using TexDifferentialType = std::array<TexCoordType, Dimension>;

        virtual ~BasicTexture() = default;

        virtual TexSampleType Eval(const TexCoordType& coord,
                                   const TexDifferentialType& differential) = 0;

        TexSampleType Eval(const TexCoordType& coord)
        {
            // TODO: determine a good default
            return Eval(coord, {});
        }
    };

    template <typename T>
    using Texture2D = BasicTexture<T, 2>;

    template <typename T>
    using Texture3D = BasicTexture<T, 3>;
} // namespace usami
