#include "GameObject.h"
#include "Component.h"
#include "Transform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"

GameObject::GameObject(const std::string& name) : name(name), active(true), parent(nullptr) {
    CreateComponent(ComponentType::TRANSFORM);
}

GameObject::~GameObject() {
    components.clear();
    componentOwners.clear();

    for (auto* child : children) {
        delete child;
    }
    children.clear();
}

Component* GameObject::CreateComponent(ComponentType type) {
    Component* newComponent = nullptr;

    switch (type) {
    case ComponentType::TRANSFORM:
        if (GetComponent(ComponentType::TRANSFORM) != nullptr) {
            LOG("GameObject has a transform");
            return GetComponent(ComponentType::TRANSFORM);
        }
        newComponent = new Transform(this);
        break;

    case ComponentType::MESH:
        newComponent = new ComponentMesh(this);
        break;

    case ComponentType::MATERIAL:
        if (GetComponent(ComponentType::MATERIAL) != nullptr) {
            LOG("GameObject has a material ");
            return GetComponent(ComponentType::MATERIAL);
        }
        newComponent = new ComponentMaterial(this);
        break;

    default:
        LOG("Unknown component");
        return nullptr;
    }

    if (newComponent) {
        componentOwners.push_back(std::unique_ptr<Component>(newComponent));
        components.push_back(newComponent);
    }

    return newComponent;
}

Component* GameObject::GetComponent(ComponentType type) const {
    for (auto* comp : components) {
        if (comp->GetType() == type) {
            return comp;
        }
    }
    return nullptr;
}

std::vector<Component*> GameObject::GetComponentsOfType(ComponentType type) const {
    std::vector<Component*> result;
    for (auto* comp : components) {
        if (comp->GetType() == type) {
            result.push_back(comp);
        }
    }
    return result;
}

void GameObject::AddChild(GameObject* child) {
    if (child && child != this) {
        if (child->parent) {
            child->parent->RemoveChild(child);
        }

        child->parent = this;
        children.push_back(child);
    }
}

void GameObject::RemoveChild(GameObject* child) {
    auto it = std::find(children.begin(), children.end(), child);
    if (it != children.end()) {
        (*it)->parent = nullptr;
        children.erase(it);
    }
}

void GameObject::SetParent(GameObject* newParent) {
    if (newParent) {
        newParent->AddChild(this);
    }
    else if (parent) {
        parent->RemoveChild(this);
    }
}

void GameObject::Update() {
    if (!active) return;

    for (auto* component : components) {
        if (component->IsActive()) {
            component->Update();
        }
    }

    for (auto* child : children) {
        child->Update();
    }
}