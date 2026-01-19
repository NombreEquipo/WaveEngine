#pragma once

#include "Component.h"
#include "AudioComponent.h"

class AudioListener : public Component, public AudioComponent {
public:
    AudioListener(GameObject* container);
    virtual ~AudioListener();

    // Override from AudioComponent
    void SetTransform() override;

    // Set as the default listener in the scene (like camara there must be a main one)
    void SetAsDefaultListener();

private:
    bool isDefault = true;
};


