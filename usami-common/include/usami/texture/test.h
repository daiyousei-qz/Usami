#pragma once
#include "usami/texture.h"
#include <functional>

namespace usami
{
    class TestTexture : public BasicTexture<Vec3f, 2>
    {
    private:
        std::function<Vec3f(Vec2f, Vec2f, Vec2f)> f_;

    public:
        TestTexture(std::function<Vec3f(Vec2f, Vec2f, Vec2f)> f) : f_(f)
        {
        }

        Vec3f Eval(const TexCoordType& coord, const TexDifferentialType& differential) override
        {
            return f_(coord, differential[0], differential[1]);
        }
    };
} // namespace usami
