#pragma once
#include <stdint.h>
#include <string>
#include <unordered_map>

class ConstantBuffer {
public:
	struct ConstantBufferVariableInfo {
		int offset;
		int arrayStride; // -1 if variable is not of array type
		int matrixStride; // 0 if variable is not of matrix type;
		uint32_t indexInBlock; // index of a variable inside a block
	};

	struct ConstantBufferInfo {
		int size = -1;
		int bindingPoint = -1;
		std::unordered_map<std::string, ConstantBufferVariableInfo> variablesInfo;
	};

public:
	ConstantBuffer() = default;
	virtual  ~ConstantBuffer() = default;
	virtual void setData(ConstantBufferInfo&& bufferInfo, bool isDynamic) = 0;

	// Updates data
	virtual void updateData(const std::string& variableName, const void* data, uint32_t sizeInBytes) = 0;
	uint32_t getBindingPoint() const { return specification.bindingPoint; }
	const ConstantBufferInfo& getConstantBufferSpecification() const { return specification; }

protected:
	ConstantBufferInfo specification;
};
