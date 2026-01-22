#include "Application.h"
#include "ModuleAudio.h"



ModuleAudio::ModuleAudio() : Module() {
    name = "Audio";
    audioSystem = std::make_unique<AudioSystem>();
}

ModuleAudio::~ModuleAudio() {}

bool ModuleAudio::Start() {
    return audioSystem->Awake(); // Initializes Wwise
}

bool ModuleAudio::Update() {
    audioSystem->Update();
    return true;
}

bool ModuleAudio::CleanUp() {
    return audioSystem->CleanUp();
}

void ModuleAudio::PlayAudio(AudioSource* source, AkUniqueID event) {
    if (source != nullptr)
        audioSystem->PlayEvent(event, source->goID);
    else
        LOG_CONSOLE(__FILE__, __LINE__, "There is no component Audio Source to play");
}

void ModuleAudio::PlayAudio(AudioSource* source, const wchar_t* eventName) {
    if (source != nullptr)
        audioSystem->PlayEvent(eventName, source->goID);
    else
        LOG_CONSOLE(__FILE__, __LINE__, "There is no component Audio Source to play");
}


void ModuleAudio::StopAudio(AudioSource* source, AkUniqueID event) {
    if (source != nullptr)
        audioSystem->StopEvent(event, source->goID);
    else
        LOG_CONSOLE(__FILE__, __LINE__, "Audio Error: Attempted to play Event ID %u on a NULL AudioSource!", event);
}


void ModuleAudio::PauseAudio(AudioSource* source, AkUniqueID event) {
    audioSystem->PauseEvent(event, source->goID);
}


void ModuleAudio::ResumeAudio(AudioSource* source, AkUniqueID event) {
    audioSystem->ResumeEvent(event, source->goID);
}

void ModuleAudio::SetSwitch(AudioSource* source, AkSwitchGroupID switchGroup, AkSwitchStateID switchState)
{
    audioSystem->SetSwitch(switchGroup, switchState, source->goID);
}