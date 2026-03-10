
#include "EditorWindow.h"
#include "Cubemap.h"
#include "Globals.h"
#include <string>

class ResourceCubemap;
class Cubemap;

class CubemapEditorWindow : public EditorWindow
{
public:
    CubemapEditorWindow();
    ~CubemapEditorWindow();

    void SetCubemapToEdit(UID cubemapUID);

    void Draw() override;
    void Save();

private:
    bool DrawTextureSlot(const char* label, UID& currentUID, Cubemap* cubemap);

    UID currentCubemapUID = 0;
    ResourceCubemap* resCubemap = nullptr;
    Cubemap* editingCubemap = nullptr;
};
