#pragma once
#include "usami/mesh.h"
#include "usami/math/math.h"
#include "usami/ray/ray.h"
#include "usami/ray/shape/triangle.h"
#include "usami/ray/bbox.h"
#include "usami/ray/primitive.h"
#include "ranges"

namespace usami::ray
{
    class TriangleMesh
    {
    private:
        SceneMesh* mesh_;

        // unique_ptr<BvhNode> bvh_;

        Matrix4 model_to_world_;
        Matrix4 world_to_model_;

    public:
        void foo()
        {
        }
    };
} // namespace usami::ray
