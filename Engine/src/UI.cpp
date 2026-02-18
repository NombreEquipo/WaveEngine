
#ifndef NS_APP_FRAMEWORK_EXPORTS
    #define NS_APP_FRAMEWORK_EXPORTS
#endif
#ifndef NS_APP_FRAMEWORK
    #define NS_APP_FRAMEWORK
#endif

#define NS_BUILD_RENDERER_GL

#include "UI.h"

#include "NoesisPCH.h"
#include "NsCore/Noesis.h"


UI::UI()
{
	LOG_DEBUG("UI Constructor");
}

UI::~UI()
{

}


bool UI::Start()
{
    Noesis::SetLogHandler([](const char*, uint32_t, uint32_t level, const char*, const char* msg)
    {
            // [TRACE] [DEBUG] [INFO] [WARNING] [ERROR]
            const char* prefixes[] = { "T", "D", "I", "W", "E" };
            LOG_DEBUG("[NOESIS/%s] %s\n", prefixes[level], msg);
    });

    Noesis::GUI::Init();

    return true;
}

bool UI::Update()
{
    return true;
}

bool UI::CleanUp()
{
    return true;
}

bool UI::PreUpdate()
{
    return true;
}