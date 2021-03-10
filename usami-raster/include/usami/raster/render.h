#pragma once
#include "usami/camera.h"
#include "usami/raster/canvas.h"
#include "usami/raster/context.h"
#include "usami/mesh.h"
#include <functional>

namespace usami::raster
{
    void RenderSceneNode(const SceneNode& node)
    {
        auto& canvas  = RenderingContext::Current().GetCanvas();
        auto& vshader = RenderingContext::Current().GetVertexShader();
        auto& fshader = RenderingContext::Current().GetFregmentShader();

        RenderingContext::Current().PushModelTransform(node.transform);

        if (node.mesh != nullptr)
        {
            const auto& mesh = *node.mesh;
            for (int iface = 0; iface < mesh.num_face; ++iface)
            {
                TriangleDesc tri = mesh.GetTriangle(iface);

                Vec3f p[3] = {Vec3f{tri.vertices[0]}, Vec3f{tri.vertices[1]},
                              Vec3f{tri.vertices[2]}};

                Vertex v[3];
                for (int ivertex = 0; ivertex < 3; ++ivertex)
                {
                    Vec3f normal = tri.has_normal ? Vec3f{tri.normals[ivertex]}
                                                  : Cross(p[1] - p[0], p[2] - p[0]).Normalize();
                    Vec2f tex_coord = tri.has_tex_coord ? Vec2f{tri.tex_coords[ivertex]} : 0.f;

                    v[ivertex] = vshader.Run(p[ivertex], normal, tex_coord);
                }

                canvas.Rasterize(v[0], v[1], v[2], fshader);
            }
        }

        for (const auto& child : node.children)
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

        Matrix4 world_to_screen = ComputeWorldToRasterTransform(
            camera, {res_x, res_y}, CameraProjectionType::Perspective, z_near, z_far);

        RenderingContext::Initialize(&canvas, world_to_screen);
        RenderingContext::Current().SetVertexShader(DefaultVertexShader::Instance());
        RenderingContext::Current().SetFregmentShader(DefaultFregmentShader::Instance());
        render();
        RenderingContext::Finalize();
    }

} // namespace usami::raster
