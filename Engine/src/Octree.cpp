#include "Octree.h"
#include "GameObject.h"
#include "Transform.h"
#include "ComponentMesh.h"
#include "Log.h"

OctreeNode::OctreeNode(const glm::vec3& min, const glm::vec3& max, int maxObjects, int maxDepth, int currentDepth)
    : box_min(min)
    , box_max(max)
    , max_objects(maxObjects)
    , max_depth(maxDepth)
    , current_depth(currentDepth)
{
    for (int i = 0; i < 8; ++i)
    {
        children[i] = nullptr;
    }
}

OctreeNode::~OctreeNode()
{
    Clear();
}

void OctreeNode::Clear()
{
    // Clear all children recursively
    for (int i = 0; i < 8; ++i)
    {
        if (children[i] != nullptr)
        {
            delete children[i];
            children[i] = nullptr;
        }
    }

    // Clear objects list
    objects.clear();
}

bool OctreeNode::Insert(GameObject* obj)
{
    if (obj == nullptr)
        return false;

    // TODO: Check if object is within node bounds
    // For now, just add it

    // If we're a leaf and have space, add to this node
    if (IsLeaf() && objects.size() < max_objects)
    {
        objects.push_back(obj);
        return true;
    }

    // If we're a leaf but full, subdivide
    if (IsLeaf() && current_depth < max_depth)
    {
        Subdivide();
        RedistributeObjects();
    }

    // If we have children, try to add to appropriate child
    if (!IsLeaf())
    {
        // TODO: Find which child(ren) the object belongs to
        // For now, add to this node
        objects.push_back(obj);
        return true;
    }

    // We're at max depth and full, add anyway
    objects.push_back(obj);
    return true;
}

bool OctreeNode::Remove(GameObject* obj)
{
    if (obj == nullptr)
        return false;

    // Remove from this node
    auto it = std::find(objects.begin(), objects.end(), obj);
    if (it != objects.end())
    {
        objects.erase(it);
        return true;
    }

    // Try children
    if (!IsLeaf())
    {
        for (int i = 0; i < 8; ++i)
        {
            if (children[i] != nullptr && children[i]->Remove(obj))
            {
                return true;
            }
        }
    }

    return false;
}

void OctreeNode::Subdivide()
{
    glm::vec3 center = (box_min + box_max) * 0.5f;

    // Create 8 children
    // Bottom 4 (lower half in Y)
    children[0] = new OctreeNode(
        glm::vec3(box_min.x, box_min.y, box_min.z),
        glm::vec3(center.x, center.y, center.z),
        max_objects, max_depth, current_depth + 1
    );

    children[1] = new OctreeNode(
        glm::vec3(center.x, box_min.y, box_min.z),
        glm::vec3(box_max.x, center.y, center.z),
        max_objects, max_depth, current_depth + 1
    );

    children[2] = new OctreeNode(
        glm::vec3(box_min.x, box_min.y, center.z),
        glm::vec3(center.x, center.y, box_max.z),
        max_objects, max_depth, current_depth + 1
    );

    children[3] = new OctreeNode(
        glm::vec3(center.x, box_min.y, center.z),
        glm::vec3(box_max.x, center.y, box_max.z),
        max_objects, max_depth, current_depth + 1
    );

    // Top 4 (upper half in Y)
    children[4] = new OctreeNode(
        glm::vec3(box_min.x, center.y, box_min.z),
        glm::vec3(center.x, box_max.y, center.z),
        max_objects, max_depth, current_depth + 1
    );

    children[5] = new OctreeNode(
        glm::vec3(center.x, center.y, box_min.z),
        glm::vec3(box_max.x, box_max.y, center.z),
        max_objects, max_depth, current_depth + 1
    );

    children[6] = new OctreeNode(
        glm::vec3(box_min.x, center.y, center.z),
        glm::vec3(center.x, box_max.y, box_max.z),
        max_objects, max_depth, current_depth + 1
    );

    children[7] = new OctreeNode(
        glm::vec3(center.x, center.y, center.z),
        glm::vec3(box_max.x, box_max.y, box_max.z),
        max_objects, max_depth, current_depth + 1
    );
}

void OctreeNode::RedistributeObjects()
{
    // TODO: Move objects to appropriate children
    // For now, just keep them in this node
}

void OctreeNode::DebugDraw() const
{
    // TODO: Draw AABB for this node

    // Recursively draw children
    if (!IsLeaf())
    {
        for (int i = 0; i < 8; ++i)
        {
            if (children[i] != nullptr)
            {
                children[i]->DebugDraw();
            }
        }
    }
}

Octree::Octree()
    : root(nullptr)
{
}

Octree::Octree(const glm::vec3& min, const glm::vec3& max, int maxObjects, int maxDepth)
    : root(nullptr)
{
    Create(min, max, maxObjects, maxDepth);
}

Octree::~Octree()
{
    Clear();
}

void Octree::Create(const glm::vec3& min, const glm::vec3& max, int maxObjects, int maxDepth)
{
    Clear();
    root = new OctreeNode(min, max, maxObjects, maxDepth, 0);
}

void Octree::Clear()
{
    if (root != nullptr)
    {
        delete root;
        root = nullptr;
    }
}

bool Octree::Insert(GameObject* obj)
{
    if (root == nullptr)
        return false;

    return root->Insert(obj);
}

bool Octree::Remove(GameObject* obj)
{
    if (root == nullptr)
        return false;

    return root->Remove(obj);
}

void Octree::DebugDraw() const
{
    if (root != nullptr)
    {
        root->DebugDraw();
    }
}