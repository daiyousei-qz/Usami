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

        T Eval(const TexCoordType& coord, const TexDifferentialType& differential) override
        {
            return value_;
        }
    };
} // namespace usami
