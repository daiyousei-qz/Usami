#include "usami/texture/image.h"

namespace usami
{
    void Mipmap::Initialize(const uint8_t* data, size_t width, size_t height, size_t channel)
    {
        // compute size of total memory space required
        size_t w        = width;
        size_t h        = height;
        size_t total_sz = 0;
        while (w > 0 && h > 0)
        {
            levels_.push_back(LevelDesc{total_sz, w, h});
            total_sz += w * h;

            w /= 2;
            h /= 2;
        }

        // initialize buffer and metadata
        buffer_.Initialize(total_sz);

        // generate mipmap
        if (channel == 3)
        {
            const uint8_t* p_src = data;
            uint8_t* p_dst       = reinterpret_cast<uint8_t*>(buffer_.Data());
            for (int i = 0; i < width * height; ++i)
            {
                p_dst[0] = p_src[0];
                p_dst[1] = p_src[1];
                p_dst[2] = p_src[2];
                p_dst[3] = 0xff;

                p_src += 3;
                p_dst += 4;
            }
        }
        else
        {
            memcpy(buffer_.Data(), data, 4 * width * height);
        }

        size_t src_width     = width;
        size_t src_height    = height;
        const uint8_t* p_src = reinterpret_cast<const uint8_t*>(buffer_.Data());
        for (int i = 1; i < levels_.size(); ++i)
        {
            const auto& level = levels_[i];
            uint8_t* p_dst    = reinterpret_cast<uint8_t*>(buffer_.Data() + level.offset);
            ResizeImageRgba(p_src, src_width, src_height, p_dst, level.width, level.height, 4);

            p_src      = p_dst;
            src_width  = level.width;
            src_height = level.height;
        }
    }

    Vec3f Mipmap::SampleNearest(size_t level, float u, float v)
    {
        const auto& desc = levels_[level];

        size_t x = static_cast<size_t>(ResolveUV<>(u) * (desc.width - 1));
        size_t y = static_cast<size_t>(ResolveUV<>(v) * (desc.height - 1));
        return ColorToSpectrumRGB_sRGB(GetPixel(desc, x, y));
    }

    Vec3f Mipmap::SampleBilinear(size_t level, float u, float v)
    {
        const auto& desc = levels_[level];

        float u_ = ResolveUV<>(u) * (desc.width - 1);
        float v_ = ResolveUV<>(v) * (desc.height - 1);

        size_t x0 = static_cast<size_t>(u_);
        size_t y0 = static_cast<size_t>(v_);
        size_t x1 = Min(x0 + 1, desc.width - 1);
        size_t y1 = Min(y0 + 1, desc.height - 1);

        float kx = x0 == x1 ? 1.f : u_ - x0;
        float ky = y0 == y1 ? 1.f : v_ - y0;

        SpectrumRGB p00 = ColorToSpectrumRGB_sRGB(GetPixel(desc, x0, y0));
        SpectrumRGB p01 = ColorToSpectrumRGB_sRGB(GetPixel(desc, x0, y1));
        SpectrumRGB p10 = ColorToSpectrumRGB_sRGB(GetPixel(desc, x1, y0));
        SpectrumRGB p11 = ColorToSpectrumRGB_sRGB(GetPixel(desc, x1, y1));

        SpectrumRGB py0 = kx * p00 + (1.f - kx) * p01;
        SpectrumRGB py1 = kx * p10 + (1.f - kx) * p11;
        return ky * py0 + (1.f - ky) * py1;
    }
} // namespace usami
