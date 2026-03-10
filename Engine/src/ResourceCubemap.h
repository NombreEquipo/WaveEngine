#pragma once

#include "ModuleResources.h"

class Cubemap;

class ResourceCubemap : public Resource {
public:
	ResourceCubemap(UID uid);
	virtual ~ResourceCubemap();

	bool LoadInMemory() override;
	void UnloadFromMemory() override;

	Cubemap* GetCubemap() { return cubemap; }

private:
	Cubemap* cubemap = nullptr;
};