#pragma once
#include "Module.h"
#include "AudioUtility.h"
#include <vector>
#include <algorithm>
#include <Win32/AkDefaultIOHookDeferred.h>

class AudioComponent;

#define MAX_AUDIO_EVENTS 20

class AudioEvent {
public:
	AudioEvent() {
	};

	bool IsPlaying() {
		return playingID != 0;
	};

	AkPlayingID playingID;
	AkCallbackFunc eventCallback; //callback function wwise uses to fire events
};

class AudioSystem : public Module {
public:

	AudioSystem();
	~AudioSystem();

	bool Awake();

	bool Update();

	bool CleanUp();

	void SetGlobalVolume(float volume) { globalVolume = volume; }
	float GetGlobalVolume() { return globalVolume; }

	void RegisterAudioComponent(AudioComponent* component);
	void UnregisterAudioComponent(AudioComponent* component);

	void RegisterGameObject(AkGameObjectID id, const char* name);
	void UnregisterGameObject(AkGameObjectID id);
	void SetPosition(AkGameObjectID id, const glm::vec3& pos, const glm::vec3& front, const glm::vec3& top);

private:
	bool InitEngine();
	bool InitMemoryManager();
	bool InitStreamingManager();
	bool InitSoundEngine();
	bool InitSpatialAudio();
	bool InitCommunication();

	void AudioSystem::LoadBank(const wchar_t* bankName);

	float globalVolume = 100.0f;

	std::vector<AkGameObjectID> gameObjectIDs;
	std::vector<AudioEvent*> audioEvents;
	std::vector<AudioComponent*> audioComponents;

public:
	//low_level I/O implementation taken from the samples folder
	CAkDefaultIOHookDeferred g_lowLevelIO;

	//implements AK::StreamMgr::IAkFileLocationResolver + AK::StreamMgr::IAkLowLevelIOHook interfaces, 
	//and is able to load file packages generated with the File Packager utility

};
