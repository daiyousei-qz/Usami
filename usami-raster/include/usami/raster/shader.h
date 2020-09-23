#pragma once
#include "usami/math/math.h"
#include "usami/raster/context.h"

namespace usami::raster
{
    struct Vertex
    {
        // screen space position in homogeneous coordinates
        Vec4f pos_screen;
        // model space position
        Vec3f pos_model;
        // model space normal vector
        Vec3f normal;
        // texture coordinate
        Vec2f tex_coord;
    };

    class VertexShader
    {
    public:
        virtual ~VertexShader() = default;

        virtual Vertex Run(const Vec3f& pos, const Vec3f& normal, const Vec2f& tex_coord) = 0;
    };

    class DefaultVertexShader final : public VertexShader
    {
    public:
        static DefaultVertexShader* Instance() noexcept
        {
            static DefaultVertexShader shader;
            return &shader;
        }

        Vertex Run(const Vec3f& pos, const Vec3f& normal, const Vec2f& tex_coord) override
        {
            const auto& world_to_screen = RenderingContext::Current().GetModelToScreenTransform();

            Vertex result;
            result.pos_screen = world_to_screen.Apply(UpgradeVec(pos, 1.f));
            result.pos_model  = pos;
            result.normal     = normal;
            result.tex_coord  = tex_coord;
            return result;
        }
    };

    class FregmentShader
    {
    public:
        virtual ~FregmentShader() = default;

        // should return false if the pixel should be discarded, true otherwise
        virtual bool Run(const Vertex& v0, const Vertex& v1, const Vertex& v2,
                         const Vec3f& barycentric, const Vec2f& duvdx, const Vec2f& duvdy,
                         Vec3f& out_color) = 0;
    };

    class DefaultFregmentShader final : public FregmentShader
    {
    public:
        static DefaultFregmentShader* Instance() noexcept
        {
            static DefaultFregmentShader shader;
            return &shader;
        }

        bool Run(const Vertex& v0, const Vertex& v1, const Vertex& v2, const Vec3f& bar,
                 const Vec2f& duvdx, const Vec2f& duvdy, Vec3f& out_color) override
        {
            Vec3f n   = v0.normal * bar[0] + v1.normal * bar[1] + v2.normal * bar[2];
            out_color = n.Abs();

            return true;
        }
    };
} // namespace usami::raster
