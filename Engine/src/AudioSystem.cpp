#pragma comment(lib, "ws2_32.lib")

#ifndef _UCRT_NO_CPP_LIBC_MACROS
#define _UCRT_NO_CPP_LIBC_MACROS
#endif

#include <ctime>
#include <iostream>

#include "AudioSystem.h"
#include "AudioUtility.h"
#include "AudioComponent.h"
#include "Log.h"

#include <AK/SoundEngine/Common/AkSoundEngine.h>
#include <AK/SpatialAudio/Common/AkSpatialAudio.h>

#include <AK/SoundEngine/Common/IAkStreamMgr.h>
#include <AK/SoundEngine/Common/AkStreamMgrModule.h> 

#include <windows.h>

AudioSystem::AudioSystem() {

}

AudioSystem::~AudioSystem() {

}

bool AudioSystem::InitEngine() {

    LOG_DEBUG("# Initializing Audio Engine...");

    //Wwise submodules must be initialized in the following order:
    if (!InitMemoryManager()) {
        LOG_CONSOLE("Failed to initialize Wwise's Memory Manager");
        return false;
    }

    if (!InitStreamingManager()) {
        LOG_CONSOLE("Failed to initialize Wwise's Streaming Manager");
        return false;
    }

    if (!InitSoundEngine()) {
        LOG_CONSOLE("Failed to initialize Wwise's Sound Engine");
        return false;
    }
    
    if (!InitSpatialAudio()) {
        LOG_CONSOLE("Failed to initialize Wwise's Spatial Audio");
        return false;
    }

#ifndef AK_OPTIMIZED
    if (!InitCommunication()) {
        LOG_CONSOLE("Failed to initialize Wwise's Authoring Tool Communication");
        return false;
    }
#endif // AK_OPTIMIZED

    return true;

}


bool AudioSystem::InitMemoryManager() {

    AkMemSettings memSettings;
    AK::MemoryMgr::GetDefaultSettings(memSettings);

    //Init memory manager
    if (AK::MemoryMgr::Init(&memSettings) != AK_Success)
    {
        LOG_CONSOLE("Could not create the memory manager.");
        return false;
    }

    return true;
}

bool AudioSystem::InitStreamingManager() {
    
    AkStreamMgrSettings stmSettings;
    AK::StreamMgr::GetDefaultSettings(stmSettings);

    if (!AK::StreamMgr::Create(stmSettings)) return false;

    AkDeviceSettings deviceSettings;
    AK::StreamMgr::GetDefaultDeviceSettings(deviceSettings);

    // Initializing the Deferred hook
    if (g_lowLevelIO.Init(deviceSettings) != AK_Success) return false;

    g_lowLevelIO.SetBasePath(AKTEXT("../Assets/Audio/GeneratedSoundBanks"));

    return true;
}

bool AudioSystem::InitSoundEngine()
{
    //Sound Engine creation using default initialization parameters

    AkInitSettings initSettings;
    AkPlatformInitSettings platformInitSettings;
    AK::SoundEngine::GetDefaultInitSettings(initSettings);
    AK::SoundEngine::GetDefaultPlatformInitSettings(platformInitSettings);

    if (AK::SoundEngine::Init(&initSettings, &platformInitSettings) != AK_Success)
    {
        LOG_CONSOLE("Could not initialize the Sound Engine.");
        return false;
    }

    return true;

}

bool AudioSystem::InitSpatialAudio() {

    //Spatial Audio initalization using default initialization parameters
    AkSpatialAudioInitSettings settings; // The constructor fills AkSpatialAudioInitSettings with the recommended default settings. 
    if (AK::SpatialAudio::Init(settings) != AK_Success)
    {
        LOG_CONSOLE("Could not initialize the Spatial Audio.");
        return false;
    }


    return true;
}

bool AudioSystem::InitCommunication() {

    ////Necessary if we want to be able to connect the Wwise Authoring tool to the game and perform in-game mixing, profiling, and troubleshooting.

    #ifndef AK_OPTIMIZED
    // Initialize communications (not in release build!)

    AkCommSettings commSettings;
    AK::Comm::GetDefaultInitSettings(commSettings);
    if (AK::Comm::Init(commSettings) != AK_Success)
    {
        LOG_CONSOLE("Could not initialize communication.");
        return false;
    }
    #endif // AK_OPTIMIZED

    return true;
}

bool AudioSystem::Awake() {
	
	//Check engine has successfully initialized
    if (!InitEngine()) {
        LOG_CONSOLE("Failed to initialized the Audio Engine");
        return false;
    }
    else {
        LOG_CONSOLE("Successfully initialized Audio Engine");
    }

    LOG_CONSOLE("Header Version: %d", AK_WWISESDK_VERSION_MAJOR);
    LOG_CONSOLE("Build Date: %s", __DATE__);

    g_lowLevelIO.SetBasePath(AKTEXT("../Assets/Audio/GeneratedSoundBanks"));

    //load init bank
    LoadBank(BANKNAME_INIT);

    //load project specific bank
    LoadBank(BANKNAME_MUSIC);

	//create audio events
	for (size_t i = 0; i < MAX_AUDIO_EVENTS; ++i) {
        
        audioEvents.push_back(new AudioEvent);
	}

    SetGlobalVolume(globalVolume);
    return true;
}

bool AudioSystem::Update() { 
    //components push their current position to Wwise
    for (auto* component : audioComponents) {
        component->SetTransform();
    }

    //ProcessAudio() in the Sound Integration Walkthrough
    //processes bank requests, events, positions, RTPC, etc.
    AK::SoundEngine::RenderAudio();
    return true;
}

bool AudioSystem::CleanUp() {
    //Stop all audio and unload banks first
    AK::SoundEngine::StopAll();
    AK::SoundEngine::ClearBanks(); // Always clear banks before termination
    AK::SoundEngine::RenderAudio(); // Process the stop/clear commands

    // Terminate Communication FIRST 
#ifndef AK_OPTIMIZED
    AK::Comm::Term();
#endif

    // Terminate Sound Engine
    AK::SoundEngine::Term();

    g_lowLevelIO.Term();

    // Terminate Stream Manager
    if (AK::IAkStreamMgr::Get()) {
        AK::IAkStreamMgr::Get()->Destroy();
    }

    // Terminate Memory Manager LAST
    AK::MemoryMgr::Term();

    return true;

}

void AudioSystem::LoadBank(const wchar_t* bankName) {
    AkBankID bankID;
    AKRESULT eResult = AK::SoundEngine::LoadBank(bankName, bankID);

    if (eResult == AK_Success) {
        LOG_CONSOLE("Loaded Bank: %ls", bankName);
    }
    else {
        LOG_CONSOLE("Wwise Bank Error: %ls (Result Code: %d)", bankName, (int)eResult);
    }
}

void AudioSystem::RegisterAudioComponent(AudioComponent* component) {
    audioComponents.push_back(component);
}

void AudioSystem::UnregisterAudioComponent(AudioComponent* component) {
    auto it = std::find(audioComponents.begin(), audioComponents.end(), component);
    if (it != audioComponents.end()) {
        audioComponents.erase(it);
    }
}

void AudioSystem::RegisterGameObject(AkGameObjectID id, const char* name) {
    AK::SoundEngine::RegisterGameObj(id, name);
}

void AudioSystem::UnregisterGameObject(AkGameObjectID id) {
    AK::SoundEngine::UnregisterGameObj(id);
}

void AudioSystem::SetPosition(AkGameObjectID id, const glm::vec3& pos, const glm::vec3& front, const glm::vec3& top) {
    AkSoundPosition soundPos;
    soundPos.SetPosition(pos.x, pos.y, pos.z);
    soundPos.SetOrientation(front.x, front.y, front.z, top.x, top.y, top.z);
    AK::SoundEngine::SetPosition(id, soundPos);
}

//---------------AK and helpers

namespace AK
{
    class StringBuilder {
    public:
        // Precise x64 memory layout for Wwise 2025
        wchar_t* m_pszString;
        uint32_t m_uAllocSize;
        uint32_t m_uStringSize;

        void Set(const wchar_t* in_psz) {
            if (m_pszString && in_psz) {
                // We use wcsncpy to safely fill the buffer Wwise already allocated
                wcsncpy(m_pszString, in_psz, m_uAllocSize - 1);
                m_pszString[m_uAllocSize - 1] = L'\0';
                m_uStringSize = (uint32_t)wcslen(m_pszString);
            }
        }
    };

    bool ResolveGeneratedSoundBanksPath(
        AK::StringBuilder& out_path,
        wchar_t const* in_osPath,
        struct AkFileSystemFlags* in_pFlags,
        bool in_bAddTrailingSeparator)
    {
        // Since you are in 'Engine/build', this path points correctly to 'Engine/Assets'
        out_path.Set(L"../Assets/Audio/GeneratedSoundBanks");
        return true;
    }

    void ConvertFileIdToFilename(wchar_t* in_psz, unsigned int in_max, unsigned int in_id, unsigned int in_flags)
    {
        swprintf(in_psz, in_max, L"%u.bnk", in_id);
    }
}