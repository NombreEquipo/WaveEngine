#pragma once
#include <string>
#include <nlohmann/json.hpp>

class GameObject;

enum class GameObjectEvent;

enum class ComponentType {
    TRANSFORM,
    MESH,
    MATERIAL,
    CAMERA,
    ROTATE,
    SCRIPT,
    PARTICLE,
    RIGIDBODY,
    COLLIDER,
    JOINT,
    UNKNOWN
};

class Component {
public:

    Component(GameObject* owner, ComponentType type);
    virtual ~Component() = default;

    virtual void Enable() {};
    virtual void Update() {};
    virtual void FixedUpdate() {};
    virtual void Disable() {};
    virtual void OnEditor() {};

    // Serialization
    virtual void Serialize(nlohmann::json& componentObj) const {};
    virtual void Deserialize(const nlohmann::json& componentObj) {};

    ComponentType GetType() const { return type; }
    virtual bool IsType(ComponentType type) = 0;
    virtual bool IsIncompatible(ComponentType type) = 0;

    bool IsActive() const { return active; }
    void SetActive(bool active) { this->active = active; }

    virtual void OnGameObjectEvent(GameObjectEvent event, Component* component) {};

public:
    GameObject* owner;
    ComponentType type;
    bool active = true;
    std::string name;
};