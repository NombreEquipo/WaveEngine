#pragma once

#include "Cubemap.h"
#include "Application.h"
#include "ModuleResources.h"
#include "ResourceTexture.h"
#include "Globals.h"

#include "Shader.h"
#include "glad/glad.h"

#include <fstream>

Cubemap::Cubemap()
{
}

Cubemap::~Cubemap()
{
    auto* resModule = Application::GetInstance().resources.get();
    
    if (rightFaceUID != 0) resModule->ReleaseResource(rightFaceUID);
    if (leftFaceUID != 0) resModule->ReleaseResource(leftFaceUID);
    if (topFaceUID != 0) resModule->ReleaseResource(topFaceUID);
    if (bottomFaceUID != 0) resModule->ReleaseResource(bottomFaceUID);
    if (frontFaceUID != 0) resModule->ReleaseResource(frontFaceUID);
    if (backFaceUID != 0) resModule->ReleaseResource(backFaceUID);

    rightFaceUID = 0;
    leftFaceUID = 0;
    topFaceUID = 0;
    bottomFaceUID = 0;
    frontFaceUID = 0;
    backFaceUID = 0;
}

void Cubemap::Bind(Shader* shader)
{
    if (!shader) return;

    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Cubemap::LoadCustomData(std::ifstream& file) {

    file.read((char*)&rightFaceUID, sizeof(UID));
    file.read((char*)&leftFaceUID, sizeof(UID));
    file.read((char*)&topFaceUID, sizeof(UID));
    file.read((char*)&bottomFaceUID, sizeof(UID));
    file.read((char*)&frontFaceUID, sizeof(UID));
    file.read((char*)&backFaceUID, sizeof(UID));

    SetRightFaceTex(rightFaceUID);
    SetLeftFaceTex(leftFaceUID);
    SetTopFaceTex(topFaceUID);
    SetBottomFaceTex(bottomFaceUID);
    SetFrontFaceTex(frontFaceUID);
    SetBackFaceTex(backFaceUID);
}

void Cubemap::SaveCustomData(std::ofstream& file) const {

    file.write((char*)&rightFaceUID, sizeof(UID));
    file.write((char*)&leftFaceUID, sizeof(UID));
    file.write((char*)&topFaceUID, sizeof(UID));
    file.write((char*)&bottomFaceUID, sizeof(UID));
    file.write((char*)&frontFaceUID, sizeof(UID));
    file.write((char*)&backFaceUID, sizeof(UID));
}

void Cubemap::SaveToJson(nlohmann::json& j) const {
    j["RightFaceUID"] = rightFaceUID;
    j["LeftFaceUID"] = leftFaceUID;
    j["TopFaceUID"] = topFaceUID;
    j["BottomFaceUID"] = bottomFaceUID;
    j["FrontFaceUID"] = frontFaceUID;
    j["BackFaceUID"] = backFaceUID;
}

void Cubemap::LoadFromJson(const nlohmann::json& j) {

    rightFaceUID = j.value("RightFaceUID", 0ull);
    leftFaceUID = j.value("LeftFaceUID", 0ull);
    topFaceUID = j.value("TopFaceUID", 0ull);
    bottomFaceUID = j.value("TopFaceUID", 0ull);
    frontFaceUID = j.value("FrontFaceUID", 0ull);
    backFaceUID = j.value("BackFaceUID", 0ull);

    SetRightFaceTex(rightFaceUID);
    SetLeftFaceTex(leftFaceUID);
    SetTopFaceTex(topFaceUID);
    SetBottomFaceTex(bottomFaceUID);
    SetFrontFaceTex(frontFaceUID);
    SetBackFaceTex(backFaceUID);
}

void Cubemap::SetRightFaceTex(UID uid) {

    if (rightFaceUID != 0) {
        Application::GetInstance().resources->ReleaseResource(rightFaceUID);
        rightFaceUID = 0;
    }

    rightFaceTex = (ResourceTexture*)Application::GetInstance().resources.get()->RequestResource(uid);

    if (rightFaceTex) rightFaceUID = uid;
}

void Cubemap::SetLeftFaceTex(UID uid) {

    if (leftFaceUID != 0) {
        Application::GetInstance().resources->ReleaseResource(leftFaceUID);
        leftFaceUID = 0;
    }

    leftFaceTex = (ResourceTexture*)Application::GetInstance().resources.get()->RequestResource(uid);

    if (leftFaceTex) leftFaceUID = uid;
}

void Cubemap::SetTopFaceTex(UID uid) {

    if (topFaceUID != 0) {
        Application::GetInstance().resources->ReleaseResource(topFaceUID);
        topFaceUID = 0;
    }

    topFaceTex = (ResourceTexture*)Application::GetInstance().resources.get()->RequestResource(uid);

    if (topFaceTex) topFaceUID = uid;
}

void Cubemap::SetBottomFaceTex(UID uid) {

    if (bottomFaceUID != 0) {
        Application::GetInstance().resources->ReleaseResource(bottomFaceUID);
        bottomFaceUID = 0;
    }

    bottomFaceTex = (ResourceTexture*)Application::GetInstance().resources.get()->RequestResource(uid);

    if (bottomFaceTex) bottomFaceUID = uid;
}

void Cubemap::SetFrontFaceTex(UID uid) {

    if (frontFaceUID != 0) {
        Application::GetInstance().resources->ReleaseResource(frontFaceUID);
        frontFaceUID = 0;
    }

    frontFaceTex = (ResourceTexture*)Application::GetInstance().resources.get()->RequestResource(uid);

    if (frontFaceTex) frontFaceUID = uid;
}

void Cubemap::SetBackFaceTex(UID uid) {

    if (backFaceUID != 0) {
        Application::GetInstance().resources->ReleaseResource(backFaceUID);
        backFaceUID = 0;
    }

    backFaceTex = (ResourceTexture*)Application::GetInstance().resources.get()->RequestResource(uid);

    if (backFaceTex) backFaceUID = uid;
}
