#pragma once
#include <string>
#include <rapidjson/document.h>

class GameObject;

enum class ComponentType {
    TRANSFORM,
    MESH,
    MATERIAL,
    CAMERA,
    ROTATE,
    UNKNOWN
};

class Component {
public:

    Component(GameObject* owner, ComponentType type);
    virtual ~Component() = default;

    virtual void Enable() {};
    virtual void Update() {};
    virtual void Disable() {};
    virtual void OnEditor() {};

    // Serialization
    virtual void Serialize(rapidjson::Value& componentObj, rapidjson::Value::AllocatorType& allocator) const {};
    virtual void Deserialize(const rapidjson::Value& componentObj) {};

    ComponentType GetType() const { return type; }
    bool IsActive() const { return active; }
    void SetActive(bool active) { this->active = active; }

public:
    GameObject* owner;
    ComponentType type;
    bool active = true;
    std::string name;
};