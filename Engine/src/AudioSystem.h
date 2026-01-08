#pragma once
#include <AK/SoundEngine/Common/AkTypes.h>

class AudioSystem
{
public:
    static bool Init();
    static void Update();
    static void Shutdown();

    static void PlayTestEvent();

private:
    static AkGameObjectID s_gameObject;
};
