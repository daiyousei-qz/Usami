#pragma once
#include "usami/color.h"
#include "usami/image.h"
#include "usami/texture.h"
#include "usami/memory/buffer.h"
#include <vector>

namespace usami
{
    class Mipmap
    {
    private:
        MemoryBuffer<ColorRGBA> buffer_;

        struct LevelDesc
        {
            size_t offset;
            size_t width;
            size_t height;
        };
        std::vector<LevelDesc> levels_;

        ColorRGBA GetPixel(const LevelDesc& desc, size_t x, size_t y) const noexcept
        {
            return buffer_.At(desc.offset + y * desc.width + x);
        }

    public:
        Mipmap() = default;
        Mipmap(const uint8_t* data, size_t width, size_t height, size_t channel)
        {
            Initialize(data, width, height, channel);
        }

        size_t NumLevels() const noexcept
        {
            return levels_.size();
        }

        void Initialize(const uint8_t* data, size_t width, size_t height, size_t channel);

        Vec3f SampleNearest(size_t level, float u, float v);

        Vec3f SampleBilinear(size_t level, float u, float v);
    };

    class ImageTexture : public Texture2D<Vec3f>
    {
    private:
        Mipmap mipmap_;
        size_t width_;
        size_t height_;

    public:
        ImageTexture(const std::string& filename)
        {
            int width, height, channel;
            auto image_buf = LoadImage(filename.c_str(), width, height, channel);

            width_  = width;
            height_ = height;
            mipmap_.Initialize(image_buf.get(), width, height, channel);
        }
        ImageTexture(const uint8_t* data, size_t width, size_t height, size_t channel)
            : mipmap_(data, width, height, channel), width_(width), height_(height)
        {
        }

        const auto& Mipmap() const noexcept
        {
            return mipmap_;
        }
        size_t Width() const noexcept
        {
            return width_;
        }
        size_t Height() const noexcept
        {
            return height_;
        }

        Vec3f Eval(Vec2f tex_coord, Vec2f differential_x, Vec2f differential_y) override
        {
            size_t level = mipmap_.NumLevels() - 1 +
                           Log2(Max(differential_x.Length(), differential_y.Length()));
            level = Clamp<size_t>(level, 0, mipmap_.NumLevels() - 1);

            return mipmap_.SampleNearest(level, tex_coord[0], tex_coord[1]);
        }
    };
} // namespace usami
