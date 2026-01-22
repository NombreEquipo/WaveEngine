#pragma once
#include "ComponentCollider.h"

class ComponentSphereCollider : public ComponentCollider {
public:
    ComponentSphereCollider(GameObject* owner);
    virtual ~ComponentSphereCollider() = default;

    void SetRadius(float newRadius);
    float GetRadius() const { return radius; }

    void OnEditor() override;

    // Aquí podrías añadir Serialize/Deserialize si lo necesitas en el futuro
    
private:
    float radius = 1.0f;
};