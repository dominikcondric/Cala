#pragma once
#include "Components.h"
#include <unordered_map>
#include <typeinfo>

class BaseConsistentComponentVector;
template<typename T> class ConsistentComponentVector;

class ComponentDatabase {
public:
	ComponentDatabase() = default;
	~ComponentDatabase() = default;

	template<typename T> ConsistentComponentVector<T>& getComponentList();
	template<typename T> std::vector<Entity>& getComponentEntityList();
	template<typename T> ComponentID getComponentID();

	std::vector<Entity>& getComponentEntityListByID(ComponentID compID)
	{
		return componentEntityList[compID];
	}

	BaseConsistentComponentVector& getBaseComponentVectorByID(ComponentID compID) 
	{
		assert((std::size_t)compID < componentList.size()); 
		return *componentList[compID]; 
	}

private:
	std::unordered_map<std::size_t, ComponentID> componentIDMap; // Stores component identifiers (class hash codes)
	std::vector<Unique<BaseConsistentComponentVector>> componentList; // List of component lists
	std::vector<std::vector<Entity>> componentEntityList; // Stores entity IDs for every component type
};

template<typename T>
inline ConsistentComponentVector<T>& ComponentDatabase::getComponentList()
{
	return *dynamic_cast<ConsistentComponentVector<T>*>(componentList[getComponentID<T>()].get());
}

template<typename T>
inline std::vector<Entity>& ComponentDatabase::getComponentEntityList()
{
	return componentEntityList[getComponentID<T>()];
}

template<typename T>
inline ComponentID ComponentDatabase::getComponentID()
{
	const auto componentsMapIterator = componentIDMap.find(typeid(T).hash_code());
	ComponentID compID;
	if (componentsMapIterator != componentIDMap.end())
	{
		compID = componentsMapIterator->second;
	}
	else
	{
		compID = (ComponentID)componentList.size();
		componentIDMap.insert(std::make_pair(typeid(T).hash_code(), compID));
		componentList.emplace_back(std::make_unique<ConsistentComponentVector<T>>());
		componentEntityList.emplace_back();
	}

	return compID;
}
