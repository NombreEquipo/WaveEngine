#include "AudioSource.h"
#include "GameObject.h"
#include "Transform.h"
#include "Application.h"
#include "AudioSystem.h"
#include <glm/gtc/matrix_access.hpp> // Required for column extraction


AudioSource::AudioSource(GameObject* containerGO)
    : Component(containerGO, ComponentType::AUDIOSOURCE) // This sets the base owner
{
    // REMOVE 'this->owner = containerGO;'
    this->goID = (AkGameObjectID)this;
    this->GO = std::shared_ptr<GameObject>(containerGO, [](GameObject*) {});

    // Check if the base owner is valid before using it
    if (owner != nullptr) {
        AK::SoundEngine::RegisterGameObj(this->goID, owner->name.c_str());
        Application::GetInstance().audio->audioSystem->RegisterAudioComponent(this);
    }
}

AudioSource::~AudioSource()
{
    Application::GetInstance().audio->audioSystem->UnregisterAudioComponent(this);
    AK::SoundEngine::UnregisterGameObj(this->goID);
}

void AudioSource::SetTransform() {
    Transform* trans = static_cast<Transform*>(owner->GetComponent(ComponentType::TRANSFORM));
    if (trans) {
        AkSoundPosition soundPos;
        glm::vec3 pos = trans->GetPosition();
        glm::vec3 forward = trans->GetForward();
        glm::vec3 up = trans->GetUp();

        soundPos.SetPosition(pos.x, pos.y, pos.z);
        soundPos.SetOrientation(forward.x, forward.y, forward.z, up.x, up.y, up.z);

        //AK::SoundEngine::SetPosition((AkGameObjectID)owner, soundPos);
        AK::SoundEngine::SetPosition(this->goID, soundPos);
    }
}

//void AudioSource::PlayEvent(char const* eventName)
//{
//    if (eventName == nullptr || strlen(eventName) == 0) return;
//    AK::SoundEngine::PostEvent(eventName, this->goID);
//}
//
//void AudioSource::PlayEvent(AkUniqueID eventID)
//{
//    AK::SoundEngine::PostEvent(eventID, this->goID);
//}
//
//void AudioSource::StopEvent(char const* eventName)
//{
//    if (eventName == nullptr || strlen(eventName) == 0) return;
//    AK::SoundEngine::ExecuteActionOnEvent(eventName, AK::SoundEngine::AkActionOnEventType_Stop, this->goID);
//}