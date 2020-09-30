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
#include "usami/ray/integrator/path_tracing.h"

using namespace std;
using namespace usami;
using namespace usami::ray;

// template <typename T>
// concept Function = std::is_function_v<T>;

// template <typename T>
// concept MemberFunctionPtr = std::is_member_function_pointer_v<T>;

// template <Function TFunc>
// struct FunctionDecomposition;

// template <typename TRet, typename... TArgs>
// struct FunctionDecomposition<TRet(TArgs...)>
// {
//     using ParamTupleType = std::tuple<TArgs...>;

//     using ReturnType = TRet;
// };

// template <Function TFunc>
// using FunctionParamTupleType = typename FunctionDecomposition<TFunc>::ParamTupleType;

// template <Function TFunc>
// using FunctionReturnType = typename FunctionDecomposition<TFunc>::ReturnType;

// template <MemberFunctionPtr TFunc>
// struct MemberFunctionDecomposition;

// template <typename TClass, typename TRet, typename... TArgs>
// struct MemberFunctionDecomposition<TRet (TClass::*)(TArgs...)>
// {
//     using ClassType = TClass;

//     using ParamTupleType = std::tuple<TArgs...>;

//     using ApplicationParamTupleType = std::tuple<TClass, TArgs...>;

//     using ReturnType = TRet;
// };

// template <MemberFunctionPtr TFunc>
// using MemberFunctionClassType = typename MemberFunctionDecomposition<TFunc>::ClassType;

// template <MemberFunctionPtr TFunc>
// using MemberFunctionParamTupleType = typename MemberFunctionDecomposition<TFunc>::ParamTupleType;

// template <MemberFunctionPtr TFunc>
// using MemberFunctionApplicationParamTupleType =
//     typename MemberFunctionDecomposition<TFunc>::ApplicationParamTupleType;

// template <MemberFunctionPtr TFunc>
// using MemberFunctionReturnType = typename MemberFunctionDecomposition<TFunc>::ReturnType;

// template <typename TClass, Function TFunc>
// struct MakeMemberFunctionPtr;

// template <typename TClass, typename TRet, typename... TArgs>
// struct MakeMemberFunctionPtr<TClass, TRet(TArgs...)>
// {
//     using Type = TRet (TClass::*)(TArgs...);
// };

// template <typename TClass, Function TFunc>
// using MakeMemberFunctionPtrType = typename MakeMemberFunctionPtr<TClass, TFunc>::Type;

// template <typename TClass, Function TFunc, MakeMemberFunctionPtrType<TClass, TFunc> MemPtr>
// struct HasMemberFunction
//     : std::bool_constant<
//           std::is_same_v<MemberFunctionReturnType<decltype(MemPtr)>, FunctionReturnType<TFunc>>
//           && std::is_same_v<MemberFunctionParamTupleType<decltype(MemPtr)>,
//                          FunctionParamTupleType<TFunc>>>
// {
// };

int num_sample     = 8;
Point2i resolution = {200, 200};

CameraSetting camera = {
    .position = {-3, 0, 1},
    .lookat   = {1, 0, 0},
    .lookup   = {0, 0, 1},
    .fov_y    = kPi / 2,
    .aspect   = 1,
};

unique_ptr<IntegratedScene> LoadScene()
{
    auto scene = make_unique<IntegratedScene>();

    shared_ptr<Material> mat_sphere = make_shared<DiffuseMaterial>(Vec3f{.5f, .2f, .2f});
    shared_ptr<Material> mat_ground = make_shared<DiffuseMaterial>(Vec3f{.8f, .8f, .8f});

    shared_ptr<TestTexture> tex_skybox =
        make_shared<TestTexture>([](Vec2f uv, auto duvdx, auto duvdy) -> Vec3f {
            return Vec3f{.6f, .6f, .8f} * Pow(uv[1], 1.5f);
        });

    scene->AddGeometricPrimitive(Sphere{Vec3f{0, 0, 1}, 1}, mat_sphere);
    scene->AddGeometricPrimitive(Rect{Vec3f{0, 0, -1}, 10, 10}, mat_ground);
    scene->AddGeometricLight(Rect{Vec3f{0, 0, 3}, 1, 1}, {1, 1, 1}, true);
    scene->AddInfiniteAreaLight(tex_skybox, 1.f, 0.f, 1e5);

    scene->Commit();
    return scene;
}

int main()
{
    using namespace usami;
    auto model___ = ParseModel("d:/models/duck/Duck.gltf");
    constexpr Vec3f vx{1, 0, 0};
    constexpr Vec3f vy{0, 1, 0};
    constexpr Vec3f vz  = Cross(vx, vy);
    constexpr Matrix4 m = Matrix4::ChangeBasis3D({1, 0, 0}, {0, 1, 0}, {0, 0, 1}, {0, 0, 0});

    auto scene = LoadScene();
    Matrix4 world_to_screen =
        ComputeWorldToScreenTransform(camera, resolution, CameraProjectionType::Perspective, 1, 2);
    Matrix4 screen_to_world = world_to_screen.Then(Matrix4::Translate3D({0, 0, 1}))
                                  .Inverse()
                                  .Then(Matrix4::Translate3D(-camera.position));

    Vec3f p = world_to_screen.ApplyPoint({2, 0, 0});
    Vec3f v = DowngradeVecLinear(screen_to_world.Apply(Vec4f{0, 0, 0, 1})).Normalize();

    Canvas canvas{resolution.x, resolution.y};
    PathTracingIntegrator integrator{};
    RenderingContext ctx{};
    Sampler sampler{0xdeadbeef};
    for (int y = 0; y < resolution.y; ++y)
    {
        for (int x = 0; x < resolution.y; ++x)
        {
            Vec4f screen_pos = Vec4f{static_cast<float>(x), static_cast<float>(y), 0, 1};
            Vec3f ray_dir    = DowngradeVecLinear(screen_to_world.Apply(screen_pos)).Normalize();
            Ray camera_ray   = {camera.position, ray_dir};

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