#include "Backup.h"
#include "Log.h"
#include "Application.h"
#include "Time.h"
#include <filesystem>
#include <sstream>

Backup::Backup()
{
}

Backup::~Backup()
{
}

bool Backup::Start()
{
	std::filesystem::path tempScenePath = std::filesystem::current_path().parent_path()/"TempScene";

	if (!std::filesystem::exists(tempScenePath)) 
	{
		std::filesystem::create_directory(tempScenePath);
		LOG_CONSOLE("[BACKUP]TempScene directory created.");
	}
	else 
	{
		LOG_CONSOLE("[BACKUP]TempScene directory already exists.");
	}

	tempSceneDir = tempScenePath.string();
	timeSinceLastBackup = 0.0f;
	//backupCounter = 0;

	return true;
}

bool Backup::Update()
{
	timeSinceLastBackup += Application::GetInstance().time->GetRealDeltaTime();

	if (timeSinceLastBackup >= backupInterval)
	{
		PerformBackup();
		timeSinceLastBackup = 0.0f;
	}

	return true;
}

bool Backup::CleanUp()
{
	return true;
}

void Backup::PerformBackup() 
{
	std::string timestamp = GetTimestamp();
	std::string backupFilename = tempSceneDir + "/backup_" + timestamp + ".json";

	bool success = Application::GetInstance().scene->SaveScene(backupFilename);

	if (success)
	{
		LOG_CONSOLE("[BACKUP] Scene saved: %s", backupFilename.c_str());
	}
	else
	{
		LOG_CONSOLE("[BACKUP] ERROR: Failed to save scene: %s", backupFilename.c_str());
	}
}

std::string Backup::GetTimestamp()
{
	auto now = std::chrono::system_clock::now();
	auto time = std::chrono::system_clock::to_time_t(now);

	std::stringstream ss;
	ss << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S");

	return ss.str();
}