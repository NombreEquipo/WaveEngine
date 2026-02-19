#pragma once
#include "Module.h"
#include <cstdint>
#include "NsCore/Ptr.h"
#include "NsGui/IView.h"

class UI : public Module
{
public:
    UI();
    ~UI();

    bool Start() override;
    bool CleanUp() override;

    void OnResize(uint32_t width, uint32_t height);
    void SetMousePoistion(int x, int y);

    void RenderToGameFramebuffer(int width, int height);
private:
    Noesis::Ptr<Noesis::IView> m_view;
    int m_currentWidth = 0;
    int m_currentHeight = 0;
};