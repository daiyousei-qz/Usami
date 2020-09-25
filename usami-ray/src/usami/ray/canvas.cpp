#include "usami/ray/canvas.h"
#include "usami/image.h"

namespace usami::ray
{
    void Canvas::SaveImage(const std::string& filename, float scalar)
    {
        std::vector<uint8_t> image_data;
        image_data.reserve(3 * width_ * height_);

        for (int y = 0; y < height_; ++y)
        {
            for (int x = 0; x < width_; ++x)
            {
                SpectrumRGB spectrum = GetPixel(x, y) * scalar;
                SpectrumRGB color    = Linear2sRGB(ToneMap_Aces(spectrum)) * 255.f;

                image_data.push_back(static_cast<uint8_t>(color[0]));
                image_data.push_back(static_cast<uint8_t>(color[1]));
                image_data.push_back(static_cast<uint8_t>(color[2]));
            }
        }

        SavePngImage(filename.c_str(), image_data.data(), width_, height_, 3);
    }
} // namespace usami::ray
