#pragma once
#include <vector>
#include "Graphics/Camera.h"
#include <glm/vec2.hpp>
#include "Core/Utility/IterableVector.h"
#include "Components.h"
#include <type_traits>
#include "Core/Utility/ConsistentVector.h"
#include "ComponentDatabase.h"
#include <bitset>

class Scene {
public:
	Scene() = default;
	~Scene() = default;
	Entity addEntity(const std::string& entTag = "");
	void removeEntity(Entity entityID);

	template<class T> T& getComponent(Entity entityID);
	template<class T> const T& getComponent(Entity entityID) const;
	template<class T> Cala::IterableVector<Entity> getComponentEntityList() const;
	template<class T, typename ...Args> void addComponent(Entity entityID, Args&&... args);
	template<class T> void shareComponent(Entity sharingEntityID, Entity receivingEntityID);
	template<class T> bool hasComponent(Entity entityID) const;
	template<class T> void removeComponent(Entity entityID);

private:
	static const uint32_t MAX_COMPONENTS = 10;

	// Component managing
	mutable ComponentDatabase componentDB;

	// Entity managing
	Cala::ConsistentVector<std::pair<std::bitset<MAX_COMPONENTS>, std::array<ComponentIndex, MAX_COMPONENTS>>> entityComponentTable;
};

#include "ConsistentComponentVector.h"
template<class T>
inline T& Scene::getComponent(Entity entityID)
{
	static_assert(std::is_base_of<Component, T>(), "T is not derived from Component!");

	ComponentID componentID = componentDB.getComponentID<T>();
	assert(hasComponent<T>(entityID) && "Component doesn't exist!");

	ConsistentComponentVector<T>& components = componentDB.getComponentList<T>();
	return components.vector[entityComponentTable.vector[entityID].second[componentID]].component;
}

template<class T>
inline const T& Scene::getComponent(Entity entityID) const
{
	static_assert(std::is_base_of<Component, T>(), "T is not derived from Component!");

	ComponentID componentID = componentDB.getComponentID<T>();
	assert(hasComponent<T>(entityID) && "Component doesn't exist!");

	const ConsistentComponentVector<T>& components = componentDB.getComponentList<T>();
	return components.vector[entityComponentTable.vector[entityID].second[componentID]].component;
}

template<class T>
inline Cala::IterableVector<Entity> Scene::getComponentEntityList() const
{
	static_assert(std::is_base_of<Component, T>(), "T is not derived from Component!");
	
	assert(componentDB.getComponentID<T>() != -1);
	return Cala::IterableVector<Entity>(componentDB.getComponentEntityList<T>());
}

template<class T, typename ...Args>
inline void Scene::addComponent(Entity entityID, Args&& ...args)
{
	static_assert(std::is_base_of<Component, T>(), "T is not derived from Component!");

	ComponentID componentID = componentDB.getComponentID<T>();
	if (hasComponent<T>(entityID))
	{
		LOG("Component '" << typeid(T).name() << "' already exists with id=" << componentID << "!");
		return;
	}

	// Modifying component list
	ConsistentComponentVector<T>& components = componentDB.getComponentList<T>();
	std::vector<Entity>& componentEntityList = componentDB.getComponentEntityList<T>();
	components.vector.emplace_back(ComponentEntity<T>(T(std::forward<Args>(args)...), entityID));

	// Modifying component entity list
	componentEntityList.push_back(entityID);
	std::sort(componentEntityList.begin(), componentEntityList.end());

	// Modifying entity component table
	entityComponentTable.vector[entityID].first.set(componentID, true);
	entityComponentTable.vector[entityID].second[componentID] = (ComponentIndex)(components.vector.size() - 1);
}

template<class T>
inline void Scene::shareComponent(Entity sharingEntityID, Entity receivingEntityID)
{
	static_assert(std::is_base_of<Component, T>(), "T is not derived from Component!");

	assert(hasComponent<T>(sharingEntityID) && "Sharing component doesn't have requested component");

	ComponentID componentID = componentDB.getComponentID<T>();
	if (hasComponent<T>(receivingEntityID))
	{
		LOG("Component '" << typeid(T).name() << "' already exists!");
		return;
	}

	ComponentIndex sharingComponentIndex = entityComponentTable.vector[sharingEntityID].second[componentID];
	entityComponentTable.vector[receivingEntityID].first |= BIT(componentID);
	entityComponentTable.vector[receivingEntityID].second[componentID] = sharingComponentIndex;

	ConsistentComponentVector<T>& components = componentDB.getComponentList<T>();
	components.vector[sharingComponentIndex].entities.push_back(receivingEntityID);
	std::sort(components.vector[sharingComponentIndex].entities.begin(), components.vector[sharingComponentIndex].entities.end());

	std::vector<Entity>& componentEntityList = componentDB.getComponentEntityList<T>();
	componentEntityList.push_back(receivingEntityID);
	std::sort(componentEntityList.begin(), componentEntityList.end());
}

template<class T>
inline bool Scene::hasComponent(Entity entityID) const
{
	static_assert(std::is_base_of<Component, T>(), "T is not derived from Component!");

	const auto& entCompTable = entityComponentTable.vector[entityID].first;
	return entCompTable.test(componentDB.getComponentID<T>());
}

template<class T>
inline void Scene::removeComponent(Entity entityID)
{
	static_assert(std::is_base_of<Component, T>(), "T is not derived from Component!");

	ComponentID componentID = componentDB.getComponentID<T>();
	if (!hasComponent<T>(entityID))
		return;

	ConsistentComponentVector<T>& components = componentDB.getComponentList<T>();
	ComponentIndex componentIndex = entityComponentTable.vector[entityID].second[componentID];
	std::vector<Entity>& componentEntities = components.vector[componentIndex].entities; // all entities sharing this component
	componentEntities.erase(std::lower_bound(componentEntities.begin(), componentEntities.end(), entityID));

	if (componentEntities.empty())
	{
		components.erase(componentIndex);

		if (!components.vector.empty() && componentIndex != components.vector.size())
		{
			for (Entity entity : components.vector[componentIndex].entities)
			{
				entityComponentTable.vector[entity].second[componentID] = componentIndex;
			}
		}
	}

	entityComponentTable.vector[entityID].first.set(componentID, false);
	entityComponentTable.vector[entityID].second[componentID] = -1;

	std::vector<Entity>& componentEntityList = componentDB.getComponentEntityList<T>();
	componentEntityList.erase(std::lower_bound(componentEntityList.begin(), componentEntityList.end(), entityID));
}
