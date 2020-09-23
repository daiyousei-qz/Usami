#pragma once
#include "usami/model.h"
#include <vector>

namespace usami::raster
{
    class SceneObject
    {
    private:
        shared_ptr<const MeshDesc> model_;
        int geometry_id;

    public:
        SceneObject(shared_ptr<const MeshDesc> model, int geom_id) noexcept
            : model_(model), geometry_id(geom_id)
        {
        }

        const MeshDesc& Model() const noexcept
        {
            return *model_;
        }

        const GeometryDesc& Geometry() const noexcept
        {
            return model_->geometries[geometry_id];
        }

        size_t GetFaceNum() const noexcept
        {
            return Geometry().vertex_indices.size() / 3;
        }

        Vec3f GetVertex(int iface, int ivertex) const noexcept
        {
            int offset = Geometry().vertex_indices[iface * 3 + ivertex];
            return Vec3f{model_->vertices[offset * 3], model_->vertices[offset * 3 + 1],
                         model_->vertices[offset * 3 + 2]};
        }

        bool HasNormal() const noexcept
        {
            return !Geometry().normal_indices.empty();
        }

        Vec3f GetNormal(int iface, int ivertex) const noexcept
        {
            int offset = Geometry().normal_indices[iface * 3 + ivertex];
            return Vec3f{model_->normals[offset * 3], model_->normals[offset * 3 + 1],
                         model_->normals[offset * 3 + 2]};
        }

        bool HasTexCoord() const noexcept
        {
            return !Geometry().tex_coords_indices.empty();
        }

        Vec2f GetTexCoord(int iface, int ivertex) const noexcept
        {
            int offset = Geometry().tex_coords_indices[iface * 3 + ivertex];
            return Vec2f{model_->tex_coords[offset * 2], model_->tex_coords[offset * 2 + 1]};
        }
    };

    class SceneNode
    {
    private:
        std::vector<shared_ptr<SceneNode>> children_;

        shared_ptr<SceneObject> object_;

        Matrix4 transform_;

    public:
        SceneNode(shared_ptr<SceneObject> object, const Matrix4& transform = Matrix4::Identity())
            : object_(std::move(object)), transform_(transform)
        {
        }
        SceneNode(std::vector<shared_ptr<SceneNode>> children,
                  const Matrix4& transform = Matrix4::Identity())
            : children_(std::move(children)), transform_(transform)
        {
        }

        const auto& Children() const noexcept
        {
            return children_;
        }

        const auto& Object() const noexcept
        {
            return object_;
        }

        const Matrix4& Transform() const noexcept
        {
            return transform_;
        }

        void SetTransform(const Matrix4& transform)
        {
            transform_ = transform;
        }

        void AddChild(std::shared_ptr<SceneNode> node)
        {
            children_.push_back(std::move(node));
        }
    };

    // managing scene graph and assets
    class SceneGraph
    {
    private:
        shared_ptr<SceneNode> root_;

    public:
        SceneGraph(std::shared_ptr<SceneNode> root) : root_(std::move(root))
        {
        }

        SceneNode& Root() noexcept
        {
            return *root_;
        }
        const SceneNode& Root() const noexcept
        {
            return *root_;
        }
    };
} // namespace usami::raster
