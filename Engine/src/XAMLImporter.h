#pragma once
#include <string> 
#include "Globals.h"

struct XAMLData
{
    unsigned char* data     = nullptr;
    size_t         dataSize = 0;

    bool IsValid() const { return data != nullptr && dataSize > 0; }

    // Move-only to avoid accidental copies of large blobs
    XAMLData() = default;
    XAMLData(const XAMLData&)            = delete;
    XAMLData& operator=(const XAMLData&) = delete;
    XAMLData(XAMLData&& o) noexcept
        : data(o.data), dataSize(o.dataSize)
    {
        o.data = nullptr; o.dataSize = 0;
    }
    ~XAMLData() { delete[] data; }
};


#pragma pack(push, 1)
struct XAMLHeader
{
    char     magic[4]   = { 'X','A','M','B' }; // XAMl Binary
    uint32_t version    = 1;
    uint64_t dataSize   = 0;                   // bytes that follow the header
};
#pragma pack(pop)

class XAMLImporter
{
public:

    static XAMLData  ImportFromFile(const std::string& xamlPath);

    static bool      SaveToCustomFormat(const XAMLData& xamlData, UID uid);

    static XAMLData  LoadFromCustomFormat(UID uid);
};
