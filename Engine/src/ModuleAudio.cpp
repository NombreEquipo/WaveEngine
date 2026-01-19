#include "Application.h"
#include "ModuleAudio.h"
#include "Application.h"

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