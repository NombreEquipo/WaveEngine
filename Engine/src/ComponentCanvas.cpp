#include "ComponentCanvas.h"
#include "Application.h"
#include "Time.h"

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

ComponentCanvas::ComponentCanvas(GameObject* owner) : Component(owner, ComponentType::CANVAS)
{
    name = "Canvas";
    GenerateFramebuffer(width, height);
}

ComponentCanvas::~ComponentCanvas()
{
    if (view)
    {
        view->GetRenderer()->Shutdown();
        view.Reset();
    }

    // Limpiar memoria de la GPU
    if (fbo) glDeleteFramebuffers(1, &fbo);
    if (textureID) glDeleteTextures(1, &textureID);
    if (rbo) glDeleteRenderbuffers(1, &rbo);
}

bool ComponentCanvas::LoadXAML(const char* filename)
{
    Noesis::Ptr<Noesis::FrameworkElement> xaml =
        Noesis::GUI::LoadXaml<Noesis::FrameworkElement>(filename);

    if (!xaml) return false;

    view = Noesis::GUI::CreateView(xaml);
    view->SetFlags(Noesis::RenderFlags_PPAA | Noesis::RenderFlags_LCD);
    view->SetSize(width, height);

    Noesis::Ptr<Noesis::RenderDevice> device = Noesis::MakePtr<NoesisApp::GLRenderDevice>(false);
    view->GetRenderer()->Init(device);

    currentXAML = filename;
    return true;
}

void ComponentCanvas::Update()
{
    if (!view) return;

    view->Update(Application::GetInstance().time->GetTotalTime());
}

void ComponentCanvas::RenderToTexture()
{
    if (!view) return;

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, width, height);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    view->GetRenderer()->UpdateRenderTree();
    view->GetRenderer()->RenderOffscreen();
    view->GetRenderer()->Render();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glBindVertexArray(0);
    glUseProgram(0);
    glDisable(GL_SCISSOR_TEST);
    glEnable(GL_DEPTH_TEST);
}

void ComponentCanvas::Resize(int width, int height)
{
    if (width == width && height == height) return;

    width = width;
    height = height;

    if (view) view->SetSize(width, height);

    GenerateFramebuffer(width, height);
}

void ComponentCanvas::GenerateFramebuffer(int width, int height)
{
    if (fbo) glDeleteFramebuffers(1, &fbo);
    if (textureID) glDeleteTextures(1, &textureID);
    if (rbo) glDeleteRenderbuffers(1, &rbo);

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);

    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}