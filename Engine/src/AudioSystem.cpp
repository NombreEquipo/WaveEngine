#include "AudioSystem.h"

// Wwise
#include <AK/SoundEngine/Common/AkSoundEngine.h>
#include <AK/SoundEngine/Common/AkTypes.h>

// --------------------------------------------------

AkGameObjectID AudioSystem::s_gameObject = 1;

// --------------------------------------------------

bool AudioSystem::Init()
{
    AkInitSettings initSettings;
    AkPlatformInitSettings platformSettings;

    AK::SoundEngine::GetDefaultInitSettings(initSettings);
    AK::SoundEngine::GetDefaultPlatformInitSettings(platformSettings);

    if (AK::SoundEngine::Init(&initSettings, &platformSettings) != AK_Success)
        return false;

    AK::SoundEngine::RegisterGameObj(s_gameObject, "TestObject");

    return true;
}

// --------------------------------------------------

void AudioSystem::PlayTestEvent()
{
    AK::SoundEngine::PostEvent(
        "Play_acoustic_guitar_loop_91bpm",
        s_gameObject
    );
}

// --------------------------------------------------

void AudioSystem::Update()
{
    AK::SoundEngine::RenderAudio();
}

// --------------------------------------------------

void AudioSystem::Shutdown()
{
    AK::SoundEngine::UnregisterGameObj(s_gameObject);
    AK::SoundEngine::Term();
}
