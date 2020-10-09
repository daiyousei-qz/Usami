#include "usami/ray/scene/embree.h"
#include "usami/ray/material/diffuse.h"
#include "usami/ray/light/diffuse.h"
#include <embree3/rtcore.h>
#include <ranges>
#include <algorithm>

namespace usami::ray
{
    namespace
    {
        static RTCDevice embree_device = nullptr;

        RTCDevice GetEmbreeDevice()
        {
            if (embree_device == nullptr)
            {
                embree_device = rtcNewDevice(nullptr);
            }

            USAMI_REQUIRE(embree_device != nullptr);

            return embree_device;
        }

        void ReleaseEmbree()
        {
            if (embree_device != nullptr)
            {
                rtcReleaseDevice(embree_device);
                embree_device = nullptr;
            }
        }

        RTCRayHit CreateEmptyRayHit(const Ray& us_ray)
        {
            RTCRayHit result;
            auto& ray = result.ray;
            auto& hit = result.hit;

            ray.org_x = us_ray.o.x;
            ray.org_y = us_ray.o.y;
            ray.org_z = us_ray.o.z;
            ray.tnear = kTravelDistanceMin;

            ray.dir_x = us_ray.d.x;
            ray.dir_y = us_ray.d.y;
            ray.dir_z = us_ray.d.z;
            ray.time  = 0.f;

            ray.tfar  = kTravelDistanceMax;
            ray.mask  = 0u;
            ray.id    = 0u;
            ray.flags = 0u;

            hit.instID[0] = RTC_INVALID_GEOMETRY_ID;
            hit.geomID    = RTC_INVALID_GEOMETRY_ID;
            hit.primID    = RTC_INVALID_GEOMETRY_ID;

            return result;
        }
    } // namespace

    EmbreeScene::EmbreeScene()
    {
        auto device = GetEmbreeDevice();

        scene_ = rtcNewScene(device);
    }

    EmbreeScene::~EmbreeScene()
    {
        rtcReleaseScene(scene_);
    }

    void EmbreeScene::Commit()
    {
        rtcSetSceneBuildQuality(scene_, RTC_BUILD_QUALITY_HIGH);
        rtcCommitScene(scene_);

        Scene::Commit();
    }

    bool EmbreeScene::Intersect(const Ray& ray, Workspace& workspace, IntersectionInfo& isect) const
    {
        // forward intersect request to embree

        RTCIntersectContext ctx;
        rtcInitIntersectContext(&ctx);

        RTCRayHit ray_hit = CreateEmptyRayHit(ray);

        rtcIntersect1(scene_, &ctx, &ray_hit);
        unsigned geom_id = ray_hit.hit.geomID;
        unsigned prim_id = ray_hit.hit.primID;

        if (geom_id == RTC_INVALID_GEOMETRY_ID && prim_id == RTC_INVALID_GEOMETRY_ID)
        {
            return false;
        }

        const EmbreeMeshGeometry* geometry = geom_lookup_[geom_id];

        isect.t     = ray_hit.ray.tfar;
        isect.point = ray.o + isect.t * ray.d;
        isect.ng    = Vec3f{ray_hit.hit.Ng_x, ray_hit.hit.Ng_y, ray_hit.hit.Ng_z}.Normalize();

        const TriangleDesc tri_desc = geometry->Mesh().GetTriangle(prim_id);

        // override shading normal
        if (tri_desc.has_normal)
        {
            Vec3f n0 = tri_desc.normals[0];
            Vec3f n1 = tri_desc.normals[1];
            Vec3f n2 = tri_desc.normals[2];

            auto uu = ray_hit.hit.u;
            auto vv = ray_hit.hit.v;
            auto ww = 1 - uu - vv;

            // TODO: needs to be converted into world coordinate
            isect.ns = geometry->Transform().ApplyVector(ww * n0 + uu * n1 + vv * n2).Normalize();
        }
        else
        {
            isect.ns = isect.ng;
        }

        // override texture coordinate
        if (tri_desc.has_tex_coord)
        {
            Vec2f uv0 = tri_desc.tex_coords[0];
            Vec2f uv1 = tri_desc.tex_coords[1];
            Vec2f uv2 = tri_desc.tex_coords[2];

            auto uu = ray_hit.hit.u;
            auto vv = ray_hit.hit.v;
            auto ww = 1 - uu - vv;

            isect.uv = ww * uv0 + uu * uv1 + vv * uv2;
        }
        else
        {
            isect.uv = {ray_hit.hit.u, ray_hit.hit.v};
        }

        isect.iface      = prim_id;
        isect.primitive  = InstantiateTemporaryPrimitive(workspace, geom_id, prim_id, tri_desc);
        isect.area_light = geometry->GetAreaLight(prim_id);
        isect.material   = geometry->GetMaterial();

        return true;
    }

    void EmbreeScene::AddModel(shared_ptr<SceneModel> model, const Matrix4& model_to_world)
    {
        EmbreeRegisteredModel& model_registry = models_.emplace_back();

        // parse materials
        for (const auto& mat_desc : model->materials)
        {
            // TODO: add correct material
            model_registry.material_cache[mat_desc.get()] =
                arena_.Construct<DiffuseMaterial>(Vec3f{mat_desc->base_color_factor});
        }

        for (const auto& mesh : model->meshes)
        {
            // create wrapper scene to be instanced
            RTCScene instance_scene = rtcNewScene(GetEmbreeDevice());

            // create embree geometry
            RTCGeometry rtc_geom =
                rtcNewGeometry(GetEmbreeDevice(), RTCGeometryType::RTC_GEOMETRY_TYPE_TRIANGLE);

            // set triangle index buffer
            const auto& index_buf = mesh->indices;
            rtcSetSharedGeometryBuffer(rtc_geom, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3,
                                       index_buf.buffer->data.get(), index_buf.offset,
                                       index_buf.buffer->stride,
                                       index_buf.buffer->size / index_buf.buffer->stride);

            // set vertex buffer
            const auto& vertex_buf = mesh->vertices;
            rtcSetSharedGeometryBuffer(rtc_geom, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3,
                                       vertex_buf.buffer->data.get(), vertex_buf.offset,
                                       vertex_buf.buffer->stride,
                                       vertex_buf.buffer->size / vertex_buf.buffer->stride);

            // finalize geometry
            rtcCommitGeometry(rtc_geom);
            rtcAttachGeometry(instance_scene, rtc_geom);
            // rtcReleaseGeometry(rtc_geom);

            // finalize scene
            rtcSetSceneBuildQuality(instance_scene, RTC_BUILD_QUALITY_HIGH);
            rtcCommitScene(instance_scene);

            model_registry.mesh_instance_cache.insert(std::pair{mesh.get(), instance_scene});
        }

        for (const SceneNode* root_node : model->roots)
        {
            AddSceneNode(root_node, model_to_world, model_registry);
        }

        model_registry.model = std::move(model);
    }

    void EmbreeScene::AddSceneNode(const SceneNode* node, const Matrix4& parent_transform,
                                   const EmbreeRegisteredModel& registry)
    {
        const Matrix4& global_transform = parent_transform.Then(node->transform);
        if (node->mesh != nullptr)
        {
            AddMeshGeometry(node->mesh, parent_transform, registry);
        }

        for (const SceneNode* child_node : node->children)
        {
            AddSceneNode(child_node, global_transform, registry);
        }
    }

    void EmbreeScene::AddMeshGeometry(const SceneMesh* mesh, const Matrix4& model_to_world,
                                      const EmbreeRegisteredModel& registry)
    {
        EmbreeMeshGeometry* geom = arena_.Construct<EmbreeMeshGeometry>();

        // book-keeping
        geom->id_             = geom_lookup_.size();
        geom->mesh_           = mesh;
        geom->model_to_world_ = model_to_world;
        geom_lookup_.push_back(geom);

        // register geometry into embree
        RTCGeometry rtc_geom = rtcNewGeometry(GetEmbreeDevice(), RTC_GEOMETRY_TYPE_INSTANCE);
        rtcSetGeometryInstancedScene(rtc_geom, registry.mesh_instance_cache.at(mesh));

        // set model to world transformation
        // TODO: verify last row of the matrix being affine
        // auto mat = model_to_world.ToArray();
        std::array<float, 12> mat = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0};
        rtcSetGeometryTransform(rtc_geom, 0, RTC_FORMAT_FLOAT3X4_ROW_MAJOR, &mat);

        // finalize
        rtcCommitGeometry(rtc_geom);
        rtcAttachGeometryByID(scene_, rtc_geom, geom->Id());
        // rtcReleaseGeometry(rtc_geom);

        // process material
        if (mesh->material != nullptr)
        {
            geom->material = registry.material_cache.at(mesh->material);

            Vec3f emmisive_factor = mesh->material->emissive_factor;
            if (emmisive_factor.LengthSq() > kFloatEpsilon)
            {
                geom->area_lights.reserve(mesh->num_face);

                for (int i = 0; i < mesh->num_face; ++i)
                {
                    const Primitive* primitive =
                        InstantiatePrimitive(geom->Id(), i, mesh->GetTriangle(i));
                    const AreaLight* light =
                        arena_.Construct<DiffuseAreaLight>(primitive, emmisive_factor);

                    AddLightSource(light);
                    geom->area_lights.push_back(light);
                }
            }
        }
    }

    void EmbreeScene::RegisterMeshGeometry(const EmbreeMeshGeometry& geometry,
                                           const Matrix4& model_to_world)
    {
    }

    Primitive* EmbreeScene::InstantiatePrimitive(unsigned geom_id, unsigned prim_id,
                                                 const TriangleDesc& tri_desc)
    {
        auto p = arena_.Construct<EmbreeTriangle>();
        CreatePrimitiveAux(*p, geom_id, prim_id, tri_desc);

        return p;
    }

    Primitive* EmbreeScene::InstantiateTemporaryPrimitive(Workspace& workspace, unsigned geom_id,
                                                          unsigned prim_id,
                                                          const TriangleDesc& tri_desc) const
    {
        auto p = workspace.Construct<EmbreeTriangle>();
        CreatePrimitiveAux(*p, geom_id, prim_id, tri_desc);

        return p;
    }

    void EmbreeScene::CreatePrimitiveAux(EmbreeTriangle& p, unsigned geom_id, unsigned prim_id,
                                         const TriangleDesc& tri_desc) const
    {
        p.geom_id_ = geom_id;
        p.prim_id_ = prim_id;

        Vec3f v0 = tri_desc.vertices[0];
        Vec3f v1 = tri_desc.vertices[1];
        Vec3f v2 = tri_desc.vertices[2];

        p.v0_ = v0;
        p.e1_ = v1 - v0;
        p.e2_ = v2 - v0;

        p.area_ = Cross(p.e1_, p.e2_).Length() * .5f;
    }
} // namespace usami::ray
