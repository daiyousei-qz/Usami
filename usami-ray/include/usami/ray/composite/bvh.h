#pragma once
#include "usami/mesh.h"
#include "usami/ray/bbox.h"
#include "usami/ray/primitive.h"
#include "usami/ray/shape/triangle.h"
#include <algorithm>
#include <vector>

namespace usami::ray
{
    struct PrimitiveInfo
    {
        BoundingBox bbox;
        Vec3f centroid;
        size_t index;
    };

    class BasicPrimitiveCollection
    {
    private:
        std::vector<Primitive*> prims_;

    public:
        BasicPrimitiveCollection()
        {
        }
        BasicPrimitiveCollection(std::vector<Primitive*> prims) : prims_(std::move(prims))
        {
        }

        bool Intersect(int prim_offset, int num_prim, const Ray& ray, float t_min, float t_max,
                       Workspace& ws, IntersectionInfo& isect_out) const
        {
            bool hit    = false;
            float t_hit = t_max;

            for (int i = 0; i < num_prim; ++i)
            {
                if (prims_[prim_offset + i]->Intersect(ray, t_min, t_hit, ws, isect_out))
                {
                    hit   = true;
                    t_hit = isect_out.t;
                }
            }

            return hit;
        }

        std::vector<PrimitiveInfo> Prepare()
        {
            std::vector<PrimitiveInfo> result;
            result.reserve(prims_.size());

            for (size_t i = 0; i < prims_.size(); ++i)
            {
                BoundingBox bbox = prims_[i]->Bounding();
                result.push_back(
                    PrimitiveInfo{.bbox = bbox, .centroid = bbox.Centroid(), .index = i});
            }

            return result;
        }

        void Update(const std::vector<PrimitiveInfo>& v)
        {
            USAMI_ASSERT(v.size() == prims_.size());

            std::vector<Primitive*> tmp;
            tmp.resize(prims_.size());
            for (size_t i = 0; i < v.size(); ++i)
            {
                tmp[i] = prims_[v[i].index];
            }

            prims_ = tmp;
        }
    };

    // collection specialized for mesh triangles
    // TODO: reorder triangle buffer for cache locality!!!
    class MeshPrimitiveCollection
    {
    private:
        SceneMesh* mesh_;
        std::vector<int> faces_;

    public:
        MeshPrimitiveCollection()
        {
        }
        MeshPrimitiveCollection(SceneMesh* mesh) : mesh_(mesh)
        {
            faces_.reserve(mesh->num_face);
        }

        bool Intersect(int prim_offset, int num_prim, const Ray& ray, float t_min, float t_max,
                       Workspace& ws, IntersectionInfo& isect_out) const
        {
            bool hit    = false;
            float t_hit = t_max;

            for (int i = 0; i < num_prim; ++i)
            {
                int iface         = faces_[prim_offset + i];
                auto [v0, v1, v2] = mesh_->GetTriangleVertices(iface).vertices;

                if (TestIntersection(shape::Triangle{v0, v1, v2}, ray, t_min, t_hit, isect_out))
                {
                    hit   = true;
                    t_hit = isect_out.t;

                    isect_out.iface = iface;
                }
            }

            return hit;
        }

        std::vector<PrimitiveInfo> Prepare()
        {
            std::vector<PrimitiveInfo> result;
            result.reserve(mesh_->num_face);

            for (size_t i = 0; i < mesh_->num_face; ++i)
            {
                auto [v0, v1, v2] = mesh_->GetTriangleVertices(i).vertices;
                BoundingBox bbox  = shape::Triangle{v0, v1, v2}.Bounding();
                result.push_back(
                    PrimitiveInfo{.bbox = bbox, .centroid = bbox.Centroid(), .index = i});
            }

            return result;
        }

        void Update(const std::vector<PrimitiveInfo>& v)
        {
            faces_.reserve(v.size());
            for (size_t i = 0; i < v.size(); ++i)
            {
                faces_.push_back(v[i].index);
            }
        }
    };

    template <typename PrimitiveCollection>
    class BasicBvhComposite : public IntersectableEntity
    {
    private:
        struct BvhNode
        {
            BoundingBox bbox;

            unique_ptr<BvhNode> left;
            unique_ptr<BvhNode> right;

            int prim_info_begin;
            int prim_info_end;
            int partition_axis;
        };

        struct LinearBvhNode
        {
            Array3f bbox_p_min;
            Array3f bbox_p_max;

            uint16_t axis;
            uint16_t prim_num;
            union
            {
                // prim_num > 0
                uint32_t prim_offset;

                // prim_num == 0, NOTE left child is the next node in the array
                uint32_t right_child_index;
            };
        };

        static_assert(sizeof(LinearBvhNode) == 32);

        PrimitiveCollection prims_;

        // seialized binary tree for bvh
        std::vector<LinearBvhNode> bvh_nodes_;

    public:
        using PrimitiveCollectionType = PrimitiveCollection;

        BasicBvhComposite(PrimitiveCollection prims) : prims_(std::move(prims))
        {
            std::vector<PrimitiveInfo> prim_info_vec = prims_.Prepare();

            unique_ptr<BvhNode> bvh_root = BuildBvh(prim_info_vec, 0, prim_info_vec.size());

            prims_.Update(prim_info_vec);

            // TODO: reserve space to cut unnecessary allocation
            RegisterBvh(bvh_root.get());
        }

        BoundingBox Bounding() const
        {
            return ComputeBoundingBox(0);
        }

        bool Intersect(const Ray& ray, float t_min, float t_max, Workspace& ws,
                       IntersectionInfo& isect) const noexcept
        {
            return IntersectAux(0, ray, t_min, t_max, ws, isect);
        }

    private:
        unique_ptr<BvhNode> BuildBvh(std::span<PrimitiveInfo> prims, int prim_info_begin,
                                     int prim_info_end)
        {
            USAMI_ASSERT(prim_info_end > prim_info_begin);

            BoundingBox bbox_total = prims[prim_info_begin].bbox;
            for (const PrimitiveInfo& prim_info :
                 prims.subspan(prim_info_begin + 1, prim_info_end - prim_info_begin - 1))
            {
                bbox_total = UnionBBox(bbox_total, prim_info.bbox);
            }

            int partition_axis = 0;
            {
                Vec3f extents     = bbox_total.Extents();
                float axis_extent = extents[0];

                for (int i = 1; i < 3; ++i)
                {
                    if (extents[i] > axis_extent)
                    {
                        partition_axis = i;
                        axis_extent    = extents[i];
                    }
                }
            }

            if (prim_info_end - prim_info_begin <= 8)
            {
                return make_unique<BvhNode>(BvhNode{.bbox            = bbox_total,
                                                    .left            = nullptr,
                                                    .right           = nullptr,
                                                    .prim_info_begin = prim_info_begin,
                                                    .prim_info_end   = prim_info_end,
                                                    .partition_axis  = partition_axis});
            }
            else
            {
                int prim_info_mid = std::midpoint(prim_info_begin, prim_info_end);

                PrimitiveInfo* ptr_begin = prims.data() + prim_info_begin;
                PrimitiveInfo* ptr_end   = prims.data() + prim_info_end;
                PrimitiveInfo* ptr_mid   = prims.data() + prim_info_mid;

                // split with equal count
                std::nth_element(
                    ptr_begin, ptr_mid, ptr_end,
                    [partition_axis](const PrimitiveInfo& lhs, const PrimitiveInfo& rhs) {
                        return lhs.centroid[partition_axis] < rhs.centroid[partition_axis];
                    });

                // split with midpoint
                // std::partition(ptr_begin, ptr_end,
                //                [partition_axis, mid = bbox_total.Centroid()[partition_axis]](
                //                    const PrimitiveInfo& prim_info) {
                //                    return prim_info.centroid[partition_axis] < mid;
                //                });

                size_t num_prim_left  = std::distance(ptr_begin, ptr_mid);
                size_t num_prim_right = std::distance(ptr_mid, ptr_end);
                return make_unique<BvhNode>(
                    BvhNode{.bbox            = bbox_total,
                            .left            = BuildBvh(prims, prim_info_begin, prim_info_mid),
                            .right           = BuildBvh(prims, prim_info_mid, prim_info_end),
                            .prim_info_begin = -1,
                            .prim_info_end   = -1,
                            .partition_axis  = partition_axis});
            }
        }

        BoundingBox ComputeBoundingBox(uint32_t inode) const
        {
            return BoundingBox{bvh_nodes_[inode].bbox_p_min, bvh_nodes_[inode].bbox_p_max};
        }

        uint32_t RegisterBvh(BvhNode* node)
        {
            USAMI_ASSERT(node != nullptr);

            uint32_t index = bvh_nodes_.size();
            bvh_nodes_.push_back(LinearBvhNode{
                .bbox_p_min = node->bbox.p_min.Array(),
                .bbox_p_max = node->bbox.p_max.Array(),
                .axis       = static_cast<uint16_t>(node->partition_axis),
                .prim_num   = static_cast<uint16_t>(node->prim_info_end - node->prim_info_begin),
            });

            if (node->left != nullptr)
            {
                RegisterBvh(node->left.get());
                bvh_nodes_[index].right_child_index = RegisterBvh(node->right.get());
            }
            else
            {
                bvh_nodes_[index].prim_offset = node->prim_info_begin;
            }

            return index;
        }

        bool IntersectAux(uint32_t inode, const Ray& ray, float t_min, float t_max, Workspace& ws,
                          IntersectionInfo& isect) const noexcept
        {
            if (float t_hit; !ComputeBoundingBox(inode).Occlude(ray, t_min, t_max, t_hit))
            {
                return false;
            }

            const LinearBvhNode& node = bvh_nodes_[inode];
            if (node.prim_num != 0)
            {
                // leaf
                return prims_.Intersect(node.prim_offset, node.prim_num, ray, t_min, t_max, ws,
                                        isect);
            }
            else
            {
                // non-leaf
                uint32_t ichild_left  = inode + 1;
                uint32_t ichild_right = node.right_child_index;

                IntersectionInfo isect_tmp;

                bool hit_left  = IntersectAux(ichild_left, ray, t_min, t_max, ws, isect);
                bool hit_right = IntersectAux(ichild_right, ray, t_min, t_max, ws, isect_tmp);
                if (hit_right)
                {
                    if (!hit_left || isect_tmp.t < isect.t)
                    {
                        isect = isect_tmp;
                    }
                }
                else if (!hit_left)
                {
                    return false;
                }

                return true;
            }
        }
    };

    using BvhComposite     = BasicBvhComposite<BasicPrimitiveCollection>;
    using MeshBvhComposite = BasicBvhComposite<MeshPrimitiveCollection>;
} // namespace usami::ray
