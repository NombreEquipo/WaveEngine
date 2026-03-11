#pragma once
#include "ModuleResources.h"
#include <string>

class ResourceXAML : public Resource
{
public:
    explicit ResourceXAML(UID uid);
    ~ResourceXAML() override;

    // Resource interface
    bool LoadInMemory() override;
    void UnloadFromMemory() override;

    const unsigned char* GetData()   const { return data; }
    size_t               GetSize()   const { return dataSize; }
    bool                 IsValid()   const { return data != nullptr && dataSize > 0; }

private:
    unsigned char* data     = nullptr;
    size_t         dataSize = 0;
};
