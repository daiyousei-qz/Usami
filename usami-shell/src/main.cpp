#include <cstdio>
#include "usami/math/basic_vector.h"
#include "usami/camera.h"
#include "usami/mesh.h"
#include "usami/texture.h"
#include "usami/texture/test.h"
#include "usami/texture/image.h"
#include "usami/ray/canvas.h"
#include "usami/ray/material/diffuse.h"
#include "usami/ray/shape/sphere.h"
#include "usami/ray/shape/rect.h"
#include "usami/ray/shape/disk.h"
#include "usami/ray/scene/integrated.h"
#include "usami/ray/scene/embree.h"
#include "usami/ray/integrator/path_tracing.h"

using namespace std;
using namespace usami;
using namespace usami::ray;

unique_ptr<IntegratedScene> LoadScene()
{
    auto scene = make_unique<IntegratedScene>();

    shared_ptr<Material> mat_sphere = make_shared<DiffuseMaterial>(Vec3f{.5f, .2f, .2f});
    shared_ptr<Material> mat_ground = make_shared<DiffuseMaterial>(Vec3f{.8f, .8f, .8f});

    shared_ptr<TestTexture> tex_skybox =
        make_shared<TestTexture>([](Vec2f uv, auto duvdx, auto duvdy) -> Vec3f {
            return Vec3f{.6f, .6f, .8f} * Pow(uv[1], 1.5f);
        });

    for (int yy = -10; yy <= 10; yy += 2)
    {
        for (int xx = 0; xx <= 4; xx += 2)
        {
            scene->AddGeometricPrimitive(
                Sphere{Vec3f{static_cast<float>(xx), static_cast<float>(yy), 0}, .6f}, mat_sphere);
        }
    }

    // scene->AddGeometricPrimitive(Sphere{Vec3f{0, 0, 1}, 1}, mat_sphere);
    scene->AddGeometricPrimitive(Rect{Vec3f{0, 0, -1}, 10, 10}, mat_ground);
    scene->AddGeometricLight(Rect{Vec3f{0, 0, 3}, 1, 1}, {1, 1, 1}, true);
    scene->AddInfiniteAreaLight(tex_skybox, 1.f, 0.f, 1e5);

    scene->Commit();
    return scene;
}

unique_ptr<EmbreeScene> LoadEmbreeScene()
{
    auto scene = make_unique<EmbreeScene>();

    // auto model___ = ParseModel("d:/models/duck/Duck.gltf");
    auto model___ = ParseModel("d:/wolf.glb", true);
    scene->AddModel(model___);

    shared_ptr<TestTexture> tex_skybox =
        make_shared<TestTexture>([](Vec2f uv, auto duvdx, auto duvdy) -> Vec3f {
            return Vec3f{.6f, .6f, .8f} * Pow(uv[1], 1.5f);
        });
    scene->AddInfiniteAreaLight(tex_skybox, 1.f, 0.f, 1e5);

    scene->Commit();

    return scene;
}

int main()
{
    using namespace usami;

    int num_sample     = 4;
    Point2i resolution = {400, 300};

    CameraSetting camera = {
        .position = {-5, 0, 1},
        .lookat   = {1, 0, 0},
        .lookup   = {0, 0, 1},
        .fov_y    = kPi / 4,
        .aspect   = 4.f / 3.f,
    };

    const auto& [forward, upward, rightward] =
        ComputeCameraOrientation(camera.lookat, camera.lookup);
    float ky = Tan(camera.fov_y / 2);
    float kx = ky * camera.aspect;

    auto scene = LoadScene();

    Matrix4 world_to_screen =
        ComputeWorldToRasterTransform(camera, resolution, CameraProjectionType::Perspective, 1, 2);
    Matrix4 screen_to_world = world_to_screen.Inverse();

    Canvas canvas{resolution.x, resolution.y};
    PathTracingIntegrator integrator{};
    RenderingContext ctx{};
    Sampler sampler{0xdeadbeef};

    for (int y = 0; y < resolution.y; ++y)
    {
        for (int x = 0; x < resolution.x; ++x)
        {
            auto [dx, dy] = sampler.Get2D().Array();
            Vec3f screen_pos =
                Vec3f{static_cast<float>(x) - .5f + dx, static_cast<float>(y) - .5f + dy, 0};
            Vec3f ray_dir = screen_to_world.ApplyPoint(screen_pos) - camera.position;
            // Vec3f ray_dir = forward +
            //                 (static_cast<float>(x) / resolution.x - .5f) * 2 * kx * rightward +
            //                 (static_cast<float>(y) / resolution.y - .5f) * 2 * ky * -upward;
            Ray camera_ray = {camera.position, ray_dir.Normalize()};

            for (int i = 0; i < num_sample; ++i)
            {
                SpectrumRGB pixel_radiance = integrator.Li(ctx, sampler, *scene, camera_ray);
                canvas.AppendPixel(x, y, pixel_radiance);
            }
        }
    }

    canvas.SaveImage("d:/usami-test.png", 1.f / num_sample);
    printf("hello world!");
}