#include "usami/raster/canvas.h"
#include "usami/color.h"

namespace usami::raster
{
    void Canvas::Rasterize(const Vertex& v0, const Vertex& v1, const Vertex& v2,
                           FregmentShader& shader)
    {
        using namespace std;

        Vec3f p0 = DowngradeVec(v0.pos_screen);
        Vec3f p1 = DowngradeVec(v1.pos_screen);
        Vec3f p2 = DowngradeVec(v2.pos_screen);

        // s*AB + t*AC where A, B, C respectively are v0, v1, v2
        Vec3f edge_s = p1 - p0;
        Vec3f edge_t = p2 - p0;
        if (edge_s.X() * edge_t.Y() - edge_s.Y() * edge_t.X() == 0.f)
        {
            return;
        }

        Matrix3 to_st_space = Matrix3::Projection2D({edge_s.X(), edge_s.Y()},
                                                    {edge_t.X(), edge_t.Y()}, {p0.X(), p0.Y()});

        float render_width  = static_cast<float>(width_);
        float render_height = static_cast<float>(height_);

        float min_x = clamp(ceil(min({p0.X(), p1.X(), p2.X()})), 0.f, render_width - 1);
        float min_y = clamp(ceil(min({p0.Y(), p1.Y(), p2.Y()})), 0.f, render_height - 1);
        float max_x = clamp(floor(max({p0.X(), p1.X(), p2.X()})), 0.f, render_width - 1);
        float max_y = clamp(floor(max({p0.Y(), p1.Y(), p2.Y()})), 0.f, render_height - 1);

        Vec3f left_top_uv     = to_st_space.Apply({min_x, min_y, 1.f});
        auto [dsdx, dtdx, _1] = to_st_space.Apply({1.f, 0.f, 0.f}).Array();
        auto [dsdy, dtdy, _2] = to_st_space.Apply({0.f, 1.f, 0.f}).Array();
        auto dzds             = edge_s.Z();
        auto dzdt             = edge_t.Z();

        Vec2f duvds = v1.tex_coord - v0.tex_coord;
        Vec2f duvdt = v2.tex_coord - v0.tex_coord;

        int min_xi = static_cast<int>(min_x);
        int min_yi = static_cast<int>(min_y);
        int max_xi = static_cast<int>(max_x);
        int max_yi = static_cast<int>(max_y);
        for (int y = min_yi; y <= max_yi; ++y)
        {
            for (int x = min_xi; x <= max_xi; ++x)
            {
                float s = left_top_uv.X() + (x - min_xi) * dsdx + (y - min_yi) * dsdy;
                float t = left_top_uv.Y() + (x - min_xi) * dtdx + (y - min_yi) * dtdy;
                if (s < 0.f || t < 0.f || s + t > 1.f)
                {
                    continue;
                }

                float z = p0.Z() + s * dzds + t * dzdt;
                if (!ZTest(x, y, z))
                {
                    continue;
                }

                Vec3f bar_screen = {1 - s - t, s, t};

                float a = (1 - s - t) / v0.pos_screen.W();
                float b = s / v1.pos_screen.W();
                float c = t / v2.pos_screen.W();
                float k = 1 / (a + b + c);

                Vec3f bar_world = {a * k, b * k, c * k};
                Vec2f duvdx =
                    duvds * dsdx * (k / v1.pos_screen.W()) + duvdt * dtdx * (k / v2.pos_screen.W());
                Vec2f duvdy =
                    duvds * dsdy * (k / v1.pos_screen.W()) + duvdt * dtdy * (k / v2.pos_screen.W());

                Vec3f color;
                if (shader.Run(v0, v1, v2, bar_world, duvdx, duvdy, color))
                {
                    UpdatePixel(x, y, color, z);
                }
            }
        }
    }
} // namespace usami::raster
