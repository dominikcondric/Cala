#pragma once
#include <vector>
#include <cassert>

namespace Cala {
	template<typename T>
	class IterableVector {
	public:
		using Iterator = typename std::vector<T>::iterator;
		using ConstIterator = typename std::vector<T>::const_iterator;
	public:
		IterableVector(std::vector<T>& _vector) : vector(_vector) {}
		~IterableVector() = default;

		const T& operator[](uint32_t index) const
		{
			assert(index < vector.size());
			return vector[index];
		}

		Iterator begin() const
		{
			return vector.begin();
		}

		Iterator end() const
		{
			return vector.end();
		}

		ConstIterator cbegin() const
		{
			return vector.cbegin();
		}

		ConstIterator cend() const
		{
			return vector.cend();
		}

		size_t getSize() const
		{
			return vector.size();
		}

	private:
		std::vector<T>& vector;
	};
}
