#pragma once

#include <AK/SoundEngine/Common/AkMemoryMgr.h>			// Memory Manager interface
#include <AK/SoundEngine/Common/AkMemoryMgrModule.h>				// Default memory manager

#include <AK/SoundEngine/Common/IAkStreamMgr.h>			// Streaming Manager
#include <AK/Tools/Common/AkPlatformFuncs.h>			// Thread defines
//#include <AK/Common/AkFilePackageLowLevelIODeferred.h>		// Sample low-level I/O implementation

#include <AK/SoundEngine/Common/AkSoundEngine.h>		// Sound engine


#include <AK/SpatialAudio/Common/AkSpatialAudio.h>		// Spatial Audio


#ifndef AK_OPTIMIZED
#include <AK/Comm/AkCommunication.h>
#endif  AK_OPTIMIZED


#include "Module.h"
#include "AudioSystem.h"
#include <memory>

class ModuleAudio : public Module {
public:
    ModuleAudio();
    ~ModuleAudio();

    bool Start() override;
    bool Update() override;
    bool CleanUp() override;

    std::unique_ptr<AudioSystem> audioSystem;
};
