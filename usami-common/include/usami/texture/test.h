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

        Vec3f Eval(Vec2f tex_coord, Vec2f duvdx, Vec2f duvdy) override
        {
            return f_(tex_coord, duvdx, duvdy);
        }
    };
} // namespace usami
