#include <cstdio>
#include "usami/math/basic_vector.h"
#include "usami/camera.h"
#include "usami/mesh.h"
#include "usami/texture.h"
#include "usami/texture/test.h"
#include "usami/texture/image.h"
#include "usami/ray/canvas.h"
#include "usami/ray/camera.h"
#include "usami/ray/material/diffuse.h"
#include "usami/ray/shape/sphere.h"
#include "usami/ray/shape/rect.h"
#include "usami/ray/shape/disk.h"
#include "usami/ray/scene/integrated.h"
#include "usami/ray/scene/embree.h"
#include "usami/ray/integrator/path_tracing.h"
#include "usami/ray/primitive/mesh.h"

using namespace std;
using namespace usami;
using namespace usami::ray;

auto model___ = ParseModel("d:/Box.glb", true);
// auto model___ = ParseModel("d:/models/duck/Duck.gltf");

unique_ptr<IntegratedScene> LoadScene()
{
    auto scene = make_unique<IntegratedScene>();

    shared_ptr<Material> mat_sphere = make_shared<DiffuseMaterial>(Vec3f{.5f, .2f, .2f});
    shared_ptr<Material> mat_ground = make_shared<DiffuseMaterial>(Vec3f{.8f, .8f, .8f});

    shared_ptr<TestTexture> tex_skybox =
        make_shared<TestTexture>([](Vec2f uv, auto duvdx, auto duvdy) -> Vec3f {
            return Vec3f{.6f, .6f, .8f} * Pow(uv[1], 1.5f);
        });

    for (int yy = -6; yy <= 6; yy += 2)
    {
        for (int xx = 0; xx <= 4; xx += 2)
        {
            // scene->AddGeometricPrimitive(
            //     shape::Sphere{Vec3f{static_cast<float>(xx), static_cast<float>(yy), 0}, .6f},
            //     mat_sphere);
        }
    }

    // scene->AddGeometricPrimitive(Sphere{Vec3f{0, 0, 1}, 1}, mat_sphere);
    // scene->AddMeshPrimitive(model___->meshes[0].get(), nullptr,
    //                         Matrix4::RotateY3D(.5f).Then(Matrix4::RotateZ3D(.5f)));
    scene->AddMeshPrimitive(model___->meshes[0].get(), nullptr,
                            Matrix4::Matrix4::Scale3D(.01f)
                                //.Then(Matrix4::RotateZ3D(1.f))
                                .Then(Matrix4::RotateY3D(1.5f))
                                .Then(Matrix4::Translate3D({0, -1, 1})));
    scene->AddGeometricPrimitive(shape::Rect{Vec3f{0, 0, -1}, 10, 10}, mat_ground);
    scene->AddGeometricLight(shape::Rect{Vec3f{0, 0, 3}, 1, 1}, {1, 1, 1}, true);
    scene->AddInfiniteAreaLight(tex_skybox, 1.f, 0.f, 1e5);

    scene->Commit();
    return scene;
}

unique_ptr<EmbreeScene> LoadEmbreeScene()
{
    auto scene = make_unique<EmbreeScene>();

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

    CameraSetting camera_setting = {
        .position = {-5, 0, 1},
        .lookat   = {1, 0, 0},
        .lookup   = {0, 0, 1},
        .fov_y    = kPi / 4,
        .aspect   = 4.f / 3.f,
    };

    auto scene = LoadEmbreeScene();

    Canvas canvas{resolution.x, resolution.y};
    PerspectiveCamera camera{camera_setting, resolution};
    PathTracingIntegrator integrator{};
    RenderingContext ctx{};
    Sampler sampler{0xdeadbeef};

    for (int y = 0; y < resolution.y; ++y)
    {
        for (int x = 0; x < resolution.x; ++x)
        {
            Ray camera_ray = camera.SpawnRay({x, y}, sampler.Get2D());

            for (int i = 0; i < num_sample; ++i)
            {
                SpectrumRGB pixel_radiance = integrator.Li(ctx, sampler, *scene, camera_ray);
                canvas.AppendPixel(x, y, pixel_radiance);
            }
        }
    }

    canvas.SaveImage("d:/usami-test.png", 1.f / num_sample);
}