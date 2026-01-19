#pragma once

#include "Component.h"
#include "AudioComponent.h"
#include <string>

class AudioSource : public Component, public AudioComponent {
public:
    AudioSource(GameObject* container);
    virtual ~AudioSource();

    //override from AudioComponent
    void SetTransform() override;

    //Play sound by its name
    void PlayEvent(const char* eventName);

    //Play sound by its id
    void PlayEvent(AkUniqueID eventID);

    void StopEvent(const char* eventName);

public:
    std::string eventName = "";
    bool playOnAwake = false;
    float volume = 1.0f;
};