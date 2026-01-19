#include "AudioListener.h"
#include "GameObject.h"
#include "Transform.h"
#include "Application.h"
#include "AudioSystem.h"

AudioListener::AudioListener(GameObject* containerGO)
    : Component(containerGO, ComponentType::LISTENER)
{
    this->owner = containerGO;
    this->goID = (AkGameObjectID)this;

    AK::SoundEngine::RegisterGameObj(this->goID, "MainListener");
    AK::SoundEngine::SetDefaultListeners(&this->goID, 1);

    Application::GetInstance().audio->audioSystem->RegisterAudioComponent(this);
}

AudioListener::~AudioListener()
{
    Application::GetInstance().audio->audioSystem->UnregisterAudioComponent(this);
    AK::SoundEngine::UnregisterGameObj(this->goID);
}

void AudioListener::SetTransform() {
    Transform* trans = static_cast<Transform*>(owner->GetComponent(ComponentType::TRANSFORM));
    if (trans) {
        AkSoundPosition listenerPos;
        glm::vec3 pos = trans->GetPosition();
        glm::vec3 forward = trans->GetForward();
        glm::vec3 up = trans->GetUp();

        listenerPos.SetPosition(pos.x, pos.y, pos.z);
        listenerPos.SetOrientation(forward.x, forward.y, forward.z, up.x, up.y, up.z);

        // Register the listener as a game object and set its position
        AK::SoundEngine::SetPosition(this->goID, listenerPos);
    }
}

void AudioListener::SetAsDefaultListener()
{
    AK::SoundEngine::SetDefaultListeners(&this->goID, 1);
}