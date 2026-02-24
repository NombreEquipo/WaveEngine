#include "SelectionManager.h"
#include "GameObject.h"
#include "Log.h"

void SelectionManager::SetSelectedObject(GameObject* obj)
{
	selectedObjects.clear();

	if (obj != nullptr)
	{
		selectedObjects.push_back(obj);
		LOG_DEBUG("Selected object: %s", obj->GetName().c_str());
	}
}

void SelectionManager::AddToSelection(GameObject* obj)
{
	if (obj == nullptr) return;

	if (!IsSelected(obj))
	{
		selectedObjects.push_back(obj);
		LOG_DEBUG("Added to selection: %s (total: %d)", obj->GetName().c_str(), static_cast<int>(selectedObjects.size()));
	}
}

void SelectionManager::RemoveFromSelection(GameObject* obj)
{
	if (obj == nullptr) return;

	auto it = std::find(selectedObjects.begin(), selectedObjects.end(), obj);
	if (it != selectedObjects.end())
	{
		selectedObjects.erase(it);
		LOG_DEBUG("Removed from selection: %s (total: %d)", obj->GetName().c_str(), static_cast<int>(selectedObjects.size()));
	}
}

void SelectionManager::ToggleSelection(GameObject* obj)
{
	if (obj == nullptr) return;

	if (IsSelected(obj))
	{
		RemoveFromSelection(obj);
	}
	else
	{
		AddToSelection(obj);
	}
}

void SelectionManager::ClearSelection()
{
	if (!selectedObjects.empty())
	{
		LOG_DEBUG("Selection cleared");
		selectedObjects.clear();
	}
}

GameObject* SelectionManager::GetSelectedObject() const
{
	return selectedObjects.empty() ? nullptr : selectedObjects[0];
}
std::vector<GameObject*> SelectionManager::GetFilteredObjects()
{
	std::vector<GameObject*> filtered;

	std::unordered_set<GameObject*> selectedSet(
		selectedObjects.begin(),
		selectedObjects.end()
	);

	for (GameObject* obj : selectedObjects)
	{
		GameObject* current = obj->GetParent();
		bool skip = false;

		while (current != nullptr)
		{
			if (selectedSet.find(current) != selectedSet.end())
			{
				skip = true;
				break;
			}
			current = current->GetParent();
		}

		if (!skip)
			filtered.push_back(obj);
	}
	return filtered;
}
bool SelectionManager::IsSelected(GameObject* obj) const
{
	return std::find(selectedObjects.begin(), selectedObjects.end(), obj) != selectedObjects.end();
}