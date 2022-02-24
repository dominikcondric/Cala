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
	auto& entityComponents = entityComponentTable.vector[entityID]; // Vector containing bitset and component indices for entity with given ID
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
			// entityComponents now represent components of swapped entity so its necessary to change component entity list!
			if (entityComponents.first.test(i))
			{
				ComponentIndex compIndex = entityComponents.second[i]; 

				auto& componentEntityList = componentDB.getBaseComponentVectorByID(i).getComponentEntityList(compIndex);
				*std::lower_bound(componentEntityList.begin(), componentEntityList.end(), swappedEntity) = entityID;
				std::sort(componentEntityList.begin(), componentEntityList.end());

				auto& componentEntityIndices = componentDB.getComponentEntityListByID(i);
				*std::lower_bound(componentEntityIndices.begin(), componentEntityIndices.end(), swappedEntity) = entityID;
				std::sort(componentEntityIndices.begin(), componentEntityIndices.end());
			}
		}
	}
}
