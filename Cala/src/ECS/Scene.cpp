#include "Scene.h"
#include "ConsistentComponentVector.h"

Entity Scene::addEntity(const std::string& entTag)
{
	Entity ent = (Entity)entityComponentTable.vector.size();
	entityComponentTable.vector.emplace_back();
	entityComponentTable.vector.back().first = 0;
	entityComponentTable.vector.back().second.fill(-1);
	if (!entTag.empty())
		addComponent<TagComponent>(ent, entTag);
	else
		addComponent<TagComponent>(ent, "Entity #" + std::to_string(ent));

	return ent;
}

void Scene::removeEntity(Entity entityID)
{
	auto& entityComponents = entityComponentTable.vector[entityID];
	for (ComponentID i = 0; i < MAX_COMPONENTS; i++)
	{
		if (entityComponents.first.test(i))
		{
			(this->*componentDB.getBaseComponentVectorByID(i).getRemoveComponentFunctionPointer())(entityID);
		}
	}

	entityComponentTable.erase(entityID);

	size_t entCompTableSize = entityComponentTable.size();
	if (entCompTableSize != 0 && entCompTableSize != entityID)
	{
		Entity swappedEntity = (Entity)entCompTableSize;
		for (ComponentID i = 0; i < MAX_COMPONENTS; i++)
		{
			ComponentIndex compIndex = entityComponents.second[i];
			if (entityComponents.first.test(i))
			{
				auto& entityComponentsIndices = componentDB.getBaseComponentVectorByID(i).getComponentEntityList(compIndex);
				*std::lower_bound(entityComponentsIndices.begin(), entityComponentsIndices.end(), swappedEntity) = entityID;
				std::sort(entityComponentsIndices.begin(), entityComponentsIndices.end());

				auto& entityComponentList = componentDB.getBaseComponentVectorByID(i).getComponentEntityList(compIndex);
				*std::lower_bound(entityComponentList.begin(), entityComponentList.end(), swappedEntity) = entityID;
				std::sort(entityComponentList.begin(), entityComponentList.end());
			}
		}
	}
}
