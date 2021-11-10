#pragma once
#include <vector>
#include <stdint.h>
#include <cassert>
#include "ECS/Components.h"

namespace Cala {
	class BaseConsistentVector {
	public:
		BaseConsistentVector() = default;
		virtual ~BaseConsistentVector() = default;
		virtual void erase(uint32_t index) = 0;
		virtual size_t size() = 0;
	};

	template<typename T>
	class ConsistentVector : public BaseConsistentVector {
	public:
		using iterator = typename std::vector<T>::iterator;
		using const_iterator = typename std::vector<T>::const_iterator;

	public:
		ConsistentVector() = default;
		~ConsistentVector() override = default;

		size_t size() override
		{
			return vector.size();
		}

		void erase(uint32_t index) override
		{
			assert(index < vector.size());
			vector[index] = vector.back();
			vector.pop_back();
			if (vector.capacity() - vector.size() > 20)
				vector.shrink_to_fit();
		}

		Cala::IterableVector<T> makeIterableVector()
		{
			return Cala::IterableVector<T>(vector);
		}

	public:
		std::vector<T> vector;
	};
}
