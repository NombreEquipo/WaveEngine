#pragma once
#include "Globals.h"

#include <string>
#include "nlohmann/json.hpp"

class ResourceTexture;
class Shader;

class Cubemap
{
public:
	Cubemap();
	~Cubemap();

	void Bind(Shader* shader);

	void SaveCustomData(std::ofstream& file) const;
	void LoadCustomData(std::ifstream& file);

	void SaveToJson(nlohmann::json& j) const;
	void LoadFromJson(const nlohmann::json& j);

	void SetRightFaceTex(UID uid);
	void SetLeftFaceTex(UID uid);
	void SetTopFaceTex(UID uid);
	void SetBottomFaceTex(UID uid);
	void SetFrontFaceTex(UID uid);
	void SetBackFaceTex(UID uid);

	UID GetRightFaceUID() const { return rightFaceUID; }
	UID GetLeftFaceUID()  const { return leftFaceUID; }
	UID GetTopFaceUID()   const { return topFaceUID; }
	UID GetBottomFaceUID() const { return bottomFaceUID; }
	UID GetFrontFaceUID() const { return frontFaceUID; }
	UID GetBackFaceUID()  const { return backFaceUID; }

private:
	std::string name;

	UID rightFaceUID = 0;
	UID leftFaceUID = 0;
	UID topFaceUID = 0;
	UID bottomFaceUID = 0;
	UID frontFaceUID = 0;
	UID backFaceUID = 0;

	ResourceTexture* rightFaceTex = nullptr;
	ResourceTexture* leftFaceTex = nullptr;
	ResourceTexture* topFaceTex = nullptr;
	ResourceTexture* bottomFaceTex = nullptr;
	ResourceTexture* frontFaceTex = nullptr;
	ResourceTexture* backFaceTex = nullptr;

};