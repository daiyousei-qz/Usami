#pragma once
#include "usami/common.h"
#include "usami/memory/buffer.h"
#include "usami/color.h"

namespace usami::ray
{
    /**
     * Film buffer where a rendered scene is written
     */
    class Canvas
    {
    private:
        int width_;
        int height_;
        MemoryBuffer<float> buffer_;

    public:
        Canvas(int width, int height) : buffer_(width * height * 3), width_(width), height_(height)
        {
            USAMI_ASSERT(width > 0 && height > 0);
        }

        int Width() const noexcept
        {
            return width_;
        }
        int Height() const noexcept
        {
            return height_;
        }

        void Clear()
        {
            std::fill_n(buffer_.Data(), buffer_.Size(), 0.f);
        }

        void SetPixel(int x, int y, SpectrumRGB color)
        {
            int offset             = (y * width_ + x) * 3;
            buffer_.At(offset)     = color.X();
            buffer_.At(offset + 1) = color.Y();
            buffer_.At(offset + 2) = color.Z();
        }

        void AppendPixel(int x, int y, SpectrumRGB color)
        {
            int offset = (y * width_ + x) * 3;
            buffer_.At(offset) += color.X();
            buffer_.At(offset + 1) += color.Y();
            buffer_.At(offset + 2) += color.Z();
        }

        SpectrumRGB GetPixel(int x, int y)
        {
            int offset = (y * width_ + x) * 3;
            float r    = buffer_.At(offset);
            float g    = buffer_.At(offset + 1);
            float b    = buffer_.At(offset + 2);

            return SpectrumRGB{r, g, b};
        }

        void SaveRaw(const std::string& filename, float scalar = 1.f);
        void SaveImage(const std::string& filename, float scalar = 1.f);
    };
} // namespace usami::ray
