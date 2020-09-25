#include "usami/math/math.h"
#include "usami/color.h"
#include "usami/model.h"
#include "usami/camera.h"
#include "usami/raster/render.h"
#include "quick_imgui.h"
#include <chrono>

using namespace usami;
using namespace usami::raster;

namespace ImGui
{
    bool InputVec3f(const char* label, Vec3f* v)
    {
        bool x_changed = ImGui::InputFloat(fmt::format("{}.x", label).c_str(), &v->X());
        bool y_changed = ImGui::InputFloat(fmt::format("{}.x", label).c_str(), &v->Y());
        bool z_changed = ImGui::InputFloat(fmt::format("{}.x", label).c_str(), &v->Z());
        return x_changed || y_changed || z_changed;
    }

    bool SliderVec3f(const char* label, Vec3f* v, float v_min, float v_max)
    {
        bool x_changed =
            ImGui::SliderFloat(fmt::format("{}.x", label).c_str(), &v->X(), v_min, v_max);
        bool y_changed =
            ImGui::SliderFloat(fmt::format("{}.y", label).c_str(), &v->Y(), v_min, v_max);
        bool z_changed =
            ImGui::SliderFloat(fmt::format("{}.z", label).c_str(), &v->Z(), v_min, v_max);
        return x_changed || y_changed || z_changed;
    }
} // namespace ImGui

static constexpr int kCanvasWidth  = 600;
static constexpr int kCanvasHeight = 600;

usami::CameraSetting camera;
float z_near;
float z_far;

Vec3f RotatePivot = {0.f, 1.f, 0.f};
float RotateTheta = 0;
float Scale       = 1.f;

static std::shared_ptr<MeshDesc> model;
static std::shared_ptr<SceneGraph> scene;

void LoadScene()
{
    model = LoadModel("d:/demo/scene/bunny2/bunny2.obj");
    // model = LoadModel("d:/demo/scene/cube/cube.obj");
    scene = make_shared<SceneGraph>(make_shared<SceneNode>(
        nullptr, Matrix4::Rotate3D(RotatePivot, RotateTheta) * Matrix4::Scale3D(Scale)));
    for (int i = 0; i < model->geometries.size(); ++i)
    {
        scene->Root().AddChild(std::make_shared<SceneNode>(std::make_shared<SceneObject>(model, i),
                                                           Matrix4::Scale3D(.003f)));
    }

    camera.position = {0, 0, -3};
    z_near          = 1e-2f;
    z_far           = 1e2f;
}

class MyApp : public Application
{
private:
    using TimePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;

    std::unique_ptr<PlatformTexture> tex;

    MemoryBuffer<ColorRGBA> img_data{kCanvasWidth * kCanvasHeight};
    Canvas canvas{kCanvasWidth, kCanvasHeight};

    TimePoint t0;
    float theta              = 0;
    Matrix4 rotate_transform = Matrix4::Identity();

public:
    void UpdateCanvas()
    {
        TimePoint t1 = std::chrono::high_resolution_clock::now();
        auto timespan_s =
            std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() * 1e-3;
        theta += RotateTheta * timespan_s;
        t0 = t1;
        rotate_transform =
            rotate_transform.Then(Matrix4::Rotate3D(RotatePivot, RotateTheta * timespan_s));

        scene->Root().SetTransform(rotate_transform * Matrix4::Scale3D(Scale));
        canvas.Clear(1.f);
        raster::Render(canvas, camera, z_near, z_far, [] { RenderSceneNode(scene->Root()); });

        auto pcanvas = canvas.Buffer().Data();
        auto pimg    = img_data.Data();
        for (int i = 0; i < kCanvasWidth * kCanvasHeight; ++i)
        {
            *pimg = SpectrumRGBToColor_Linear(Vec3f(pcanvas));

            pcanvas += 3;
            pimg += 1;
        }

        tex->UpdateRgba(img_data.Data());
    }

    void Initialize() override
    {
        t0  = std::chrono::high_resolution_clock::now();
        tex = AllocateTexture(kCanvasWidth, kCanvasHeight);
        LoadScene();
        UpdateCanvas();
    }

    void Update() override
    {
        bool pivot_changed = ImGui::SliderVec3f("RotatePivot", &RotatePivot, 0.f, 1.f);
        bool theta_changed = ImGui::SliderFloat("RotateSpeed", &RotateTheta, 0.f, 3.14f);
        bool scale_changed = ImGui::InputFloat("Scale", &Scale);
        if (pivot_changed)
        {
            theta            = 0.f;
            rotate_transform = Matrix4::Identity();
        }

        UpdateCanvas();
        ImGui::Text(fmt::format("Theta = {}", theta).c_str());
        ImGui::Image(tex->Id(), {kCanvasWidth, kCanvasHeight});
    }
};

int main()
{
    MyApp app;
    return RunApplication(app);
}