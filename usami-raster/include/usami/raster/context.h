#pragma once
#include "usami/math/math.h"
#include <vector>
#include <functional>

namespace usami::raster
{
    class Canvas;
    class Camera;

    class VertexShader;
    class FregmentShader;

    class RenderingContext
    {
    private:
        std::vector<Matrix4> ModelTransformStack;

        Matrix4 WorldToScreen;

        Matrix4 ModelToWorld;
        Matrix4 ModelToScreen;

        Canvas* canvas;

        VertexShader* vshader;
        FregmentShader* fshader;

        static auto& CurrentContextPtr()
        {
            static RenderingContext* ctx = nullptr;
            return ctx;
        }

        static void Initialize(Canvas* canvas, const Matrix4& world_to_screen)
        {
            static RenderingContext ctx;

            ctx.ModelTransformStack.clear();
            ctx.WorldToScreen = world_to_screen;
            ctx.ModelToWorld  = Matrix4::Identity();
            ctx.ModelToScreen = Matrix4::Identity();

            ctx.canvas  = canvas;
            ctx.vshader = nullptr;
            ctx.fshader = nullptr;

            CurrentContextPtr() = &ctx;
        }
        static void Finalize()
        {
            CurrentContextPtr() = nullptr;
        }

        friend void Render(Canvas& canvas, const Camera& camera, std::function<void()> render);

    public:
        static RenderingContext& Current()
        {
            return *CurrentContextPtr();
        }

        void SetVertexShader(VertexShader* shader)
        {
            vshader = shader;
        }

        void SetFregmentShader(FregmentShader* shader)
        {
            fshader = shader;
        }

        void PushModelTransform(const Matrix4& transform)
        {
            ModelTransformStack.push_back(ModelToWorld);
            ModelToWorld  = transform.Then(ModelToWorld);
            ModelToScreen = ModelToWorld.Then(WorldToScreen);
        }

        void PopModelTransform()
        {
            if (!ModelTransformStack.empty())
            {
                ModelToWorld = ModelTransformStack.back();
                ModelTransformStack.pop_back();
            }
            else
            {
                ModelToWorld = Matrix4::Identity();
            }
            ModelToScreen = ModelToWorld.Then(WorldToScreen);
        }

        Canvas& GetCanvas() const noexcept
        {
            return *canvas;
        }

        const Matrix4& GetModelToWorldTransform() const noexcept
        {
            return ModelToWorld;
        }

        const Matrix4& GetModelToScreenTransform() const noexcept
        {
            return ModelToScreen;
        }

        VertexShader& GetVertexShader() const noexcept
        {
            return *vshader;
        }
        FregmentShader& GetFregmentShader() const noexcept
        {
            return *fshader;
        }
    };
} // namespace usami::raster
