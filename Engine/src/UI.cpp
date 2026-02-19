#pragma once
#include "UI.h"
#include "Application.h"
#include "Time.h"
#include "ModuleEditor.h"
#include "imgui.h"

#include "NoesisPCH.h"
#include "NsCore/Noesis.h"
#include <NsCore/RegisterComponent.h>
#include <NsCore/Package.h>

#include "NsApp/LocalFontProvider.h"
#include "NsApp/LocalXamlProvider.h"
#include "NsApp/LocalTextureProvider.h"
#include <NsApp/EventTrigger.h>
#include <NsApp/GoToStateAction.h>
#include <NsApp/InvokeCommandAction.h>
#include <NsApp/Interaction.h>

#include "NsGui/IView.h"
#include "NsGui/FrameworkElement.h"
#include "NsGui/IntegrationAPI.h"
#include "GLRenderDevice.h"

extern "C" void NsRegisterReflectionAppInteractivity();
extern "C" void NsInitPackageAppInteractivity();
extern "C" void NsShutdownPackageAppInteractivity();

UI::UI() { LOG_DEBUG("UI Constructor"); }
UI::~UI() {}

bool UI::Start()
{
    Noesis::SetLogHandler([](const char*, uint32_t, uint32_t level, const char*, const char* msg)
    {
        const char* prefixes[] = { "T", "D", "I", "W", "E" };
        LOG_DEBUG("[NOESIS/%s] %s\n", prefixes[level], msg);
    });

    Noesis::GUI::SetLicense(NS_LICENSE_NAME, NS_LICENSE_KEY);
    Noesis::GUI::Init();

    NsRegisterReflectionAppInteractivity();
    NsInitPackageAppInteractivity();

    Noesis::GUI::SetXamlProvider(Noesis::MakePtr<NoesisApp::LocalXamlProvider>("../Assets/UI"));
    Noesis::GUI::SetFontProvider(Noesis::MakePtr<NoesisApp::LocalFontProvider>("../Assets/Fonts"));
    Noesis::GUI::SetTextureProvider(Noesis::MakePtr<NoesisApp::LocalTextureProvider>("../Assets/UI/Textures"));

    return true;
}

bool UI::CleanUp()
{
    NsShutdownPackageAppInteractivity();
    Noesis::GUI::Shutdown();
    return true;
}

void UI::OnResize(uint32_t width, uint32_t height)
{
    /*if (m_view)
        m_view->SetSize(width, height);*/
}

void UI::SetMousePoistion(int x, int y)
{
	/*if (m_view)
	{
		ModuleEditor* editor = Application::GetInstance().editor.get();
		if (editor)
		{
			ImVec2 gameViewportPos = editor->gameViewportPos;
			int relativeX = x - static_cast<int>(gameViewportPos.x);
			int relativeY = y - static_cast<int>(gameViewportPos.y);
			m_view->MouseMove(relativeX, relativeY);
		}
	}*/
}
