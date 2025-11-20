#pragma once

#include <glm/glm.hpp>
#include <vector>

class GameObject;

class OctreeNode
{
public:
    OctreeNode(const glm::vec3& min, const glm::vec3& max, int maxObjects = 4, int maxDepth = 5, int currentDepth = 0);
    ~OctreeNode();

    void Clear();
    bool Insert(GameObject* obj);
    bool Remove(GameObject* obj);

    template<typename TYPE>
    void CollectIntersections(std::vector<GameObject*>& objects, const TYPE& primitive) const;

    // Debug
    void DebugDraw() const;
    const glm::vec3& GetMin() const { return box_min; }
    const glm::vec3& GetMax() const { return box_max; }

private:
    void Subdivide();
    void RedistributeObjects();
    bool IsLeaf() const { return children[0] == nullptr; }

private:
    glm::vec3 box_min;
    glm::vec3 box_max;

    std::vector<GameObject*> objects;
    OctreeNode* children[8];  // 8 children for octree

    int max_objects;    // Max objects before subdividing
    int max_depth;      // Max depth of tree
    int current_depth;  // Current depth level
};

class Octree
{
public:
    Octree();
    Octree(const glm::vec3& min, const glm::vec3& max, int maxObjects = 4, int maxDepth = 5);
    ~Octree();

    void Create(const glm::vec3& min, const glm::vec3& max, int maxObjects = 4, int maxDepth = 5);
    void Clear();
    bool Insert(GameObject* obj);
    bool Remove(GameObject* obj);

    template<typename TYPE>
    void CollectIntersections(std::vector<GameObject*>& objects, const TYPE& primitive) const;

    // Debug
    void DebugDraw() const;

private:
    OctreeNode* root;
};