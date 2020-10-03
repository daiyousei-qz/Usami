#pragma once
#include "usami/ray/bbox.h"
#include "usami/ray/primitive.h"
#include <algorithm>

namespace usami::ray
{
    struct PrimitiveInfo
    {
        BoundingBox bbox;
        Vec3f centroid;
        size_t index;
    };

    struct BvhNode
    {
        BoundingBox bbox;

        unique_ptr<BvhNode> left;
        unique_ptr<BvhNode> right;

        int prim_info_begin;
        int prim_info_end;
        int partition_axis;
    };

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

        if (prim_info_end - prim_info_begin <= 1)
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
            std::nth_element(ptr_begin, ptr_mid, ptr_end,
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

    class BvhComposite : public IntersectableEntity
    {
    private:
        std::vector<Primitive*> prims_;
        std::vector<LinearBvhNode> bvh_nodes_;

    public:
        BvhComposite(const std::vector<Primitive*>& prims)
        {
            std::vector<PrimitiveInfo> prim_info_vec;
            prim_info_vec.reserve(prims.size());
            for (size_t i = 0; i < prims.size(); ++i)
            {
                BoundingBox bbox = prims[i]->Bounding();
                prim_info_vec.push_back(
                    PrimitiveInfo{.bbox = bbox, .centroid = bbox.Centroid(), .index = i});
            }

            unique_ptr<BvhNode> bvh_root = BuildBvh(prim_info_vec, 0, prim_info_vec.size());

            prims_.reserve(prims.size());
            bvh_nodes_.reserve(prims.size());

            prims_.resize(prims.size());
            for (int i = 0; i < prims.size(); ++i)
            {
                const auto& prim_info = prim_info_vec[i];
                prims_[i]             = prims[prim_info.index];
            }

            RegisterBvh(bvh_root.get());
        }

        bool Intersect(const Ray& ray, float t_min, float t_max,
                       IntersectionInfo& isect) const noexcept
        {
            if (float t_hit; !ComputeBoundingBox(0).Occlude(ray, t_min, t_max, t_hit))
            {
                return false;
            }

            return IntersectAux(0, ray, t_min, t_max, isect);
        }

    private:
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

        // ASSUMING bbox.Occlude(ray, t_min, t_max, _)
        bool IntersectAux(uint32_t inode, const Ray& ray, float t_min, float t_max,
                          IntersectionInfo& isect) const noexcept
        {
            const LinearBvhNode& node = bvh_nodes_[inode];

            if (node.prim_num != 0)
            {
                // leaf
                bool hit    = false;
                float t_hit = t_max;

                for (const auto& prim : std::span{prims_}.subspan(node.prim_offset, node.prim_num))
                {
                    if (prim->Intersect(ray, t_min, t_hit, isect))
                    {
                        hit   = true;
                        t_hit = isect.t;
                    }
                }

                return hit;
            }
            else
            {
                // non-leaf
                BoundingBox bbox_left  = ComputeBoundingBox(inode + 1);
                BoundingBox bbox_right = ComputeBoundingBox(node.right_child_index);

                uint32_t ichild_1 = inode + 1;
                uint32_t ichild_2 = node.right_child_index;

                float t1, t2;
                bool may_hit_1 = bbox_left.Occlude(ray, t_min, t_max, t1);
                bool may_hit_2 = bbox_right.Occlude(ray, t_min, t_max, t2);

                if (may_hit_2 && (!may_hit_1 || t1 > t2))
                {
                    std::swap(ichild_1, ichild_2);
                    std::swap(t1, t2);
                    std::swap(may_hit_1, may_hit_2);
                }

                bool hit_1 = may_hit_1 && IntersectAux(ichild_1, ray, t_min, t_max, isect);
                // if (hit_1 && (!may_hit_2 || isect.t < t2))
                // {
                //     return true;
                // }

                IntersectionInfo isect_tmp;
                bool hit_2 = may_hit_2 && IntersectAux(ichild_2, ray, t_min, t_max, isect_tmp);
                if (hit_2)
                {
                    if (!hit_1 || isect_tmp.t < isect.t)
                    {
                        isect = isect_tmp;
                    }
                }
                else if (!hit_1)
                {
                    return false;
                }

                return true;
            }
        }
    };
} // namespace usami::ray
