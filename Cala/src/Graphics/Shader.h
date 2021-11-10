#pragma once
#include <string>
#include <glm/glm.hpp>
#include <vector>
#include "ConstantBuffer.h"
#include "Path.h"

class Shader {
public:
	enum class ShaderType {
		VertexShader = 0,
		FragmentShader = 1,
		GeometryShader = 2,
		ComputeShader = 3,
	};

public:
	Shader() = default;
	virtual ~Shader() = default;
	virtual void activate() const = 0;
	virtual void attachShader(ShaderType type, const std::filesystem::path& filePath) = 0;
	virtual void createProgram() = 0;
	virtual void dispatchComputeShader(uint32_t workGroupX = 1, uint32_t workGroupY = 1, uint32_t workGroupZ = 1) const = 0;
	virtual ConstantBuffer::ConstantBufferInfo getConstantBufferInfo(const std::string& bufferName) const = 0;

	// Only neccessary when block binding point isn't explicitly defined in the shader code
	virtual void attachConstantBuffer(ConstantBuffer* buffer, const std::string& bufferName) const = 0;

protected:
	uint32_t attachedShaders = 0;
};