#pragma once
#include "usami/camera.h"
#include "usami/raster/canvas.h"
#include "usami/raster/context.h"
#include "usami/raster/scene.h"
#include <functional>

namespace usami::raster
{
    void RenderSceneNode(const SceneNode& node)
    {
        auto& canvas  = RenderingContext::Current().GetCanvas();
        auto& vshader = RenderingContext::Current().GetVertexShader();
        auto& fshader = RenderingContext::Current().GetFregmentShader();

        RenderingContext::Current().PushModelTransform(node.Transform());

        if (node.Object() != nullptr)
        {
            const auto& object = *node.Object();
            for (int iface = 0; iface < object.GetFaceNum(); ++iface)
            {
                Vertex v[3];
                Vec3f p[3] = {object.GetVertex(iface, 0), object.GetVertex(iface, 1),
                              object.GetVertex(iface, 2)};

                for (int ivertex = 0; ivertex < 3; ++ivertex)
                {
                    Vec3f normal = object.HasNormal() ? object.GetNormal(iface, ivertex)
                                                      : Cross(p[1] - p[0], p[2] - p[0]).Normalize();
                    Vec2f tex_coord =
                        object.HasTexCoord() ? object.GetTexCoord(iface, ivertex) : 0.f;

                    v[ivertex] = vshader.Run(p[ivertex], normal, tex_coord);
                }

                canvas.Rasterize(v[0], v[1], v[2], fshader);
            }
        }

        for (const auto& child : node.Children())
        {
            RenderSceneNode(*child);
        }

        RenderingContext::Current().PopModelTransform();
    }

    void Render(Canvas& canvas, const CameraSetting& camera, float z_near, float z_far,
                std::function<void()> render)
    {
        int res_x = canvas.Width();
        int res_y = canvas.Height();

        Matrix4 world_to_screen = ComputeWorldToScreenTransform(
            camera, {res_x, res_y}, CameraProjectionType::Perspective, z_near, z_far);

        RenderingContext::Initialize(&canvas, world_to_screen);
        RenderingContext::Current().SetVertexShader(DefaultVertexShader::Instance());
        RenderingContext::Current().SetFregmentShader(DefaultFregmentShader::Instance());
        render();
        RenderingContext::Finalize();
    }

} // namespace usami::raster
