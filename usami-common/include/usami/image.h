#pragma once

namespace usami
{
    shared_ptr<uint8_t[]> LoadImage(const char* filename, int& width_out, int& height_out,
                                    int& channel_out, bool flip_vertically = true);

    // resize a image stored compactly in RGBA form
    void ResizeImageRgba(const uint8_t* src_data, int src_width, int src_height, uint8_t* dst_buf,
                         int new_width, int new_height, int channel);

    void SavePngImage(const char* filename, const uint8_t* data, int width, int height,
                      int channel);
} // namespace usami
