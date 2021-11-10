#pragma once
#include "Core/Utility/ConsistentVector.h"

class BaseConsistentComponentVector {
public:
	using removeComponentFun = void(Scene::*)(Entity);
public:
	BaseConsistentComponentVector() = default;
	virtual ~BaseConsistentComponentVector() = default;
	virtual std::vector<Entity>& getComponentEntityList(ComponentIndex index) = 0;
	virtual removeComponentFun getRemoveComponentFunctionPointer() const = 0;
};

template<typename T>
class ConsistentComponentVector : public BaseConsistentComponentVector, public Cala::ConsistentVector<ComponentEntity<T>> {
public:
	ConsistentComponentVector() = default;
	~ConsistentComponentVector() override = default;

	std::vector<Entity>& getComponentEntityList(ComponentIndex componentIndex) override
	{
		return Cala::ConsistentVector<ComponentEntity<T>>::vector[componentIndex].entities;
	}

	removeComponentFun getRemoveComponentFunctionPointer() const override;
};

template<typename T>
BaseConsistentComponentVector::removeComponentFun ConsistentComponentVector<T>::getRemoveComponentFunctionPointer() const
{
	return &Scene::removeComponent<T>;
}

