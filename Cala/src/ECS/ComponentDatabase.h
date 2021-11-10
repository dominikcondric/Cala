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
		assert(compID < componentList.size()); 
		return *componentList[compID]; 
	}

private:
	std::unordered_map<std::size_t, ComponentID> componentMap;
	std::vector<Unique<BaseConsistentComponentVector>> componentList;
	std::vector<std::vector<Entity>> componentEntityList;
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
	const auto componentsMapIterator = componentMap.find(typeid(T).hash_code());
	ComponentID compID;
	if (componentsMapIterator != componentMap.end())
	{
		compID = componentsMapIterator->second;
	}
	else
	{
		compID = (ComponentID)componentList.size();
		componentMap.insert(std::make_pair(typeid(T).hash_code(), compID));
		componentList.emplace_back(std::make_unique<ConsistentComponentVector<T>>());
		componentEntityList.emplace_back();
	}

	return compID;
}
