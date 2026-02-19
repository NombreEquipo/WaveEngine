#pragma once
#include "Module.h"
#include <cstdint>
#include "NsCore/Ptr.h"
#include "NsGui/IView.h"
#include "ComponentCanvas.h"

class UI : public Module
{
public:
    UI();
    ~UI();

    bool Start() override;
    bool CleanUp() override;

    void OnResize(uint32_t width, uint32_t height);
    void SetMousePoistion(int x, int y);
    std::vector<ComponentCanvas*> GetCanvas() { return canvas; };

private:
    
    std::vector<ComponentCanvas*> canvas;
};