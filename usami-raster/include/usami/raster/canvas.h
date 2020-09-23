#pragma once
#include "usami/math/math.h"
#include "usami/memory/buffer.h"
#include "usami/raster/context.h"
#include "usami/raster/shader.h"

namespace usami::raster
{
    class Canvas
    {
    private:
        int width_;
        int height_;

        MemoryBuffer<float> frame_buffer_;
        MemoryBuffer<float> zbuffer_;

    public:
        Canvas(int width, int height)
            : width_(width), height_(height), frame_buffer_(width * height * 3),
              zbuffer_(width * height)
        {
        }

        const auto& Buffer()
        {
            return frame_buffer_;
        }

        int Width() const noexcept
        {
            return width_;
        }

        int Height() const noexcept
        {
            return height_;
        }

        void Clear(float c = 0.f)
        {
            frame_buffer_.Clear(c);
            zbuffer_.Clear(std::numeric_limits<float>::max());
        }

        bool ZTest(int x, int y, float z) const noexcept
        {
            return z < zbuffer_.At(y * width_ + x);
        }

        void UpdatePixel(int x, int y, Vec3f color, float z) noexcept
        {
            int zbuf_offset     = y * width_ + x;
            int framebuf_offset = (y * width_ + x) * 3;

            zbuffer_.At(zbuf_offset)              = z;
            frame_buffer_.At(framebuf_offset)     = color[0];
            frame_buffer_.At(framebuf_offset + 1) = color[1];
            frame_buffer_.At(framebuf_offset + 2) = color[2];
        }

        void Rasterize(const Vertex& v0, const Vertex& v1, const Vertex& v2,
                       FregmentShader& shader);
    };
} // namespace usami::raster
