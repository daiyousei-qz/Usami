#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "usami/common.h"
#include <cstdint>

namespace usami
{
    shared_ptr<uint8_t[]> LoadImage(const char* filename, int& width_out, int& height_out,
                                    int& channel_out, bool flip_vertically)
    {
        stbi_set_flip_vertically_on_load(flip_vertically ? 1 : 0);

        int width, height, channel;
        auto stb_buffer = stbi_load(filename, &width, &height, &channel, 3);
        if (stb_buffer == nullptr)
        {
            return nullptr;
        }

        width_out   = width;
        height_out  = height;
        channel_out = channel;
        return shared_ptr<uint8_t[]>(stb_buffer, [](uint8_t* ptr) { stbi_image_free(ptr); });
    }

    void ResizeImageRgba(const uint8_t* src_data, int src_width, int src_height, uint8_t* dst_buf,
                         int new_width, int new_height, int channel)
    {
        int success =
            stbir_resize_uint8(src_data, src_width, src_height, channel * src_width, dst_buf,
                               new_width, new_height, channel * new_width, channel);

        USAMI_REQUIRE(success != 0);
    }

    void SavePngImage(const char* filename, const uint8_t* data, int width, int height, int channel)
    {
        int success = stbi_write_png(filename, width, height, channel, data, 0);

        USAMI_REQUIRE(success != 0);
    }

} // namespace usami
