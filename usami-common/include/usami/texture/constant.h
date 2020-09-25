#pragma once
#include "usami/texture.h"

namespace usami
{
    template <VecType T = Vec3f, size_t Dimension = 2>
    class ConstantTexture : public BasicTexture<T, Dimension>
    {
    private:
        T value_;

    public:
        ConstantTexture(T value) : value_(value)
        {
        }

        T Eval(Vec<float, Dimension> tex_coord, Vec<float, Dimension> differential_x,
               Vec<float, Dimension> differential_y) override
        {
            return value_;
        }
    };
} // namespace usami
