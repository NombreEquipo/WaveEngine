#include "ComponentSphereCollider.h"
#include <btBulletDynamicsCommon.h>
#include <imgui.h>

ComponentSphereCollider::ComponentSphereCollider(GameObject* owner) 
    : ComponentCollider(owner, ComponentType::COLLIDER_SPHERE) 
{
    // Inicializamos con el radio por defecto
    shape = new btSphereShape(radius);
}

void ComponentSphereCollider::SetRadius(float newRadius) {
    radius = newRadius;
    
    // En Bullet, para cambiar el tamaño de una forma básica de forma segura, 
    // lo más común es borrar y recrear la forma.
    if (shape) {
        delete shape;
    }
    
    shape = new btSphereShape(radius);
    
    // Nota: Si este colisionador ya está asignado a un RigidBody, 
    // Bullet requiere que se notifique el cambio o se actualice la inercia.
}

void ComponentSphereCollider::OnEditor() {
    // Usamos un ID único o el nombre para el header de ImGui
    if (ImGui::CollapsingHeader("Sphere Collider", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::DragFloat("Radius", &radius, 0.1f, 0.01f, 100.0f)) {
            SetRadius(radius);
        }
    }
}