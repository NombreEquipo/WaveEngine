#include "ComponentPostProcessing.h"
#include "imgui.h"
#include <nlohmann/json.hpp>
#include "Application.h"
#include "Renderer.h"

ComponentPostProcessing::ComponentPostProcessing(GameObject* owner)
    : Component(owner, ComponentType::POSTPROCESSING)
{
    name = "Post Processing";
    Application::GetInstance().renderer->AddPostProcessing(this);
}

ComponentPostProcessing::~ComponentPostProcessing()
{
    Application::GetInstance().renderer->RemovePostProcessing(this);
}

void ComponentPostProcessing::OnEditor()
{
    if (ImGui::CollapsingHeader("Bloom", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Checkbox("Enable Bloom", &bloom.enabled);
        if (bloom.enabled)
        {
            ImGui::DragFloat("Intensity##Bloom", &bloom.intensity, 0.01f, 0.0f, 10.0f);
            ImGui::DragFloat("Threshold##Bloom", &bloom.threshold, 0.01f, 0.0f, 10.0f);
            ImGui::SliderFloat("Soft Knee##Bloom", &bloom.softKnee, 0.0f, 1.0f);
            ImGui::DragFloat("Clamp##Bloom", &bloom.clamp, 10.0f, 0.0f, 65472.0f);
            ImGui::SliderFloat("Diffusion##Bloom", &bloom.diffusion, 1.0f, 10.0f);
            ImGui::ColorEdit3("Tint##Bloom", &bloom.tint.x);
        }
    }

    if (ImGui::CollapsingHeader("Color Grading", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Checkbox("Enable Grading", &colorGrading.enabled);
        if (colorGrading.enabled)
        {
            ImGui::Text("Tone Mapping");
            const char* toneMappers[] = { "ACES", "Neutral", "None" };
            ImGui::Combo("Type##ToneMap", &colorGrading.toneMapper, toneMappers, IM_ARRAYSIZE(toneMappers));
            ImGui::DragFloat("Exposure##Grading", &colorGrading.exposure, 0.01f, 0.0f, 20.0f);

            ImGui::Separator();
            ImGui::Text("White Balance");
            ImGui::DragFloat("Temperature##WB", &colorGrading.temperature, 10.0f, -10000.0f, 10000.0f);

            // Visual gradient bar: blue (cold) -> orange (warm)
            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            ImVec2 p = ImGui::GetCursorScreenPos();
            float width = ImGui::GetContentRegionAvail().x;
            draw_list->AddRectFilledMultiColor(p, ImVec2(p.x + width, p.y + 10.0f),
                IM_COL32(0, 0, 255, 255), IM_COL32(255, 165, 0, 255),
                IM_COL32(255, 165, 0, 255), IM_COL32(0, 0, 255, 255));
            ImGui::Dummy(ImVec2(width, 10.0f));

            ImGui::DragFloat("Tint##WB", &colorGrading.tint, 1.0f, -100.0f, 100.0f);
            ImGui::ColorEdit3("Color Filter##WB", &colorGrading.colorFilter.x, ImGuiColorEditFlags_PickerHueWheel);

            ImGui::Separator();
            ImGui::Text("Global");
            ImGui::DragFloat("Saturation##Global", &colorGrading.saturation, 0.01f, 0.0f, 2.0f);
            ImGui::DragFloat("Contrast##Global", &colorGrading.contrast, 0.01f, 0.0f, 2.0f);
            ImGui::DragFloat("Gamma##Global", &colorGrading.gamma, 0.01f, 0.01f, 5.0f);
        }
    }

    if (ImGui::CollapsingHeader("Lens", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("Chromatic Aberration");
        ImGui::Checkbox("Enable##CA", &lens.chromaticAberrationEnabled);
        if (lens.chromaticAberrationEnabled)
            ImGui::SliderFloat("Intensity##CA", &lens.chromaticAberrationIntensity, 0.0f, 5.0f);

        ImGui::Separator();
        ImGui::Text("Vignette");
        ImGui::Checkbox("Enable##Vignette", &lens.vignetteEnabled);
        if (lens.vignetteEnabled)
        {
            ImGui::SliderFloat("Intensity##Vignette", &lens.vignetteIntensity, 0.0f, 1.0f);
            ImGui::SliderFloat("Smoothness##Vignette", &lens.vignetteSmoothness, 0.0f, 1.0f);
            ImGui::SliderFloat("Roundness##Vignette", &lens.vignetteRoundness, 0.0f, 1.0f);
            ImGui::ColorEdit3("Color##Vignette", &lens.vignetteColor.x);
        }
    }

    if (ImGui::CollapsingHeader("Grain", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Checkbox("Enable Grain", &grain.enabled);
        if (grain.enabled)
        {
            ImGui::SliderFloat("Intensity##Grain", &grain.intensity, 0.0f, 1.0f);
            ImGui::SliderFloat("Scale##Grain", &grain.scale, 1.0f, 10.0f);
            ImGui::Checkbox("Animated##Grain", &grain.animated);
        }
    }
}

void ComponentPostProcessing::Serialize(nlohmann::json& o) const
{
    o["bloom"] = {
        {"enabled",   bloom.enabled},
        {"intensity", bloom.intensity},
        {"threshold", bloom.threshold},
        {"softKnee",  bloom.softKnee},
        {"clamp",     bloom.clamp},
        {"diffusion", bloom.diffusion},
        {"tint",      {bloom.tint.x, bloom.tint.y, bloom.tint.z}}
    };

    o["colorGrading"] = {
        {"enabled",     colorGrading.enabled},
        {"exposure",    colorGrading.exposure},
        {"temperature", colorGrading.temperature},
        {"tint",        colorGrading.tint},
        {"contrast",    colorGrading.contrast},
        {"saturation",  colorGrading.saturation},
        {"gamma",       colorGrading.gamma},
        {"toneMapper",  colorGrading.toneMapper},
        {"colorFilter", {colorGrading.colorFilter.x, colorGrading.colorFilter.y, colorGrading.colorFilter.z}}
    };

    o["lens"] = {
        {"caEnabled",    lens.chromaticAberrationEnabled},
        {"caIntensity",  lens.chromaticAberrationIntensity},
        {"vigEnabled",   lens.vignetteEnabled},
        {"vigIntensity", lens.vignetteIntensity},
        {"vigSmoothness",lens.vignetteSmoothness},
        {"vigRoundness", lens.vignetteRoundness},
        {"vigColor",     {lens.vignetteColor.x, lens.vignetteColor.y, lens.vignetteColor.z}}
    };

    o["grain"] = {
        {"enabled",   grain.enabled},
        {"intensity", grain.intensity},
        {"scale",     grain.scale},
        {"animated",  grain.animated}
    };
}

void ComponentPostProcessing::Deserialize(const nlohmann::json& o)
{
    if (o.contains("bloom")) {
        const auto& b = o["bloom"];
        bloom.enabled = b.value("enabled", true);
        bloom.intensity = b.value("intensity", 1.0f);
        bloom.threshold = b.value("threshold", 1.0f);
        bloom.softKnee = b.value("softKnee", 0.5f);
        bloom.clamp = b.value("clamp", 65472.0f);
        bloom.diffusion = b.value("diffusion", 7.0f);
        if (b.contains("tint")) bloom.tint = glm::vec3(b["tint"][0], b["tint"][1], b["tint"][2]);
    }

    if (o.contains("colorGrading")) {
        const auto& c = o["colorGrading"];
        colorGrading.enabled = c.value("enabled", true);
        colorGrading.exposure = c.value("exposure", 1.0f);
        colorGrading.temperature = c.value("temperature", 0.0f);
        colorGrading.tint = c.value("tint", 0.0f);
        colorGrading.contrast = c.value("contrast", 1.0f);
        colorGrading.saturation = c.value("saturation", 1.0f);
        colorGrading.gamma = c.value("gamma", 1.0f);
        colorGrading.toneMapper = c.value("toneMapper", 0);
        if (c.contains("colorFilter")) colorGrading.colorFilter = glm::vec3(c["colorFilter"][0], c["colorFilter"][1], c["colorFilter"][2]);
    }

    if (o.contains("lens")) {
        const auto& l = o["lens"];
        lens.chromaticAberrationEnabled = l.value("caEnabled", false);
        lens.chromaticAberrationIntensity = l.value("caIntensity", 0.0f);
        lens.vignetteEnabled = l.value("vigEnabled", false);
        lens.vignetteIntensity = l.value("vigIntensity", 0.4f);
        lens.vignetteSmoothness = l.value("vigSmoothness", 0.2f);
        lens.vignetteRoundness = l.value("vigRoundness", 1.0f);
        if (l.contains("vigColor")) lens.vignetteColor = glm::vec3(l["vigColor"][0], l["vigColor"][1], l["vigColor"][2]);
    }

    if (o.contains("grain")) {
        const auto& g = o["grain"];
        grain.enabled = g.value("enabled", false);
        grain.intensity = g.value("intensity", 0.0f);
        grain.scale = g.value("scale", 1.0f);
        grain.animated = g.value("animated", true);
    }
}

bool ComponentPostProcessing::IsType(ComponentType type) { return type == ComponentType::POSTPROCESSING; }
bool ComponentPostProcessing::IsIncompatible(ComponentType) { return false; }