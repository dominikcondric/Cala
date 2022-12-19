#pragma once
#include <stdint.h>
#include <string>
#include <unordered_map>

typedef uint32_t GLuint;

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
	~ConstantBuffer();
	ConstantBuffer(const ConstantBuffer& other) = delete;
	ConstantBuffer(ConstantBuffer&& other) noexcept;
	ConstantBuffer& operator=(const ConstantBuffer& other) = delete;
	ConstantBuffer& operator=(ConstantBuffer&& other) noexcept;

	void setData(ConstantBufferInfo&& bufferInfo, bool isDynamic = false);

	// Updates data
	void updateData(const std::string& variableName, const void* data, uint32_t sizeInBytes) const;
	uint32_t getBindingPoint() const { return specification.bindingPoint; }
	const ConstantBufferInfo& getConstantBufferSpecification() const { return specification; }

private:
	ConstantBufferInfo specification;
#if CALA_API == CALA_API_OPENGL
	GLuint bufferID = 0;
	static std::unordered_map<uint32_t, GLuint> bindingPointsCache;
#endif
};
