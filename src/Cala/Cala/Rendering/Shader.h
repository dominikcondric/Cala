#pragma once
#include <string>
#include <glm/glm.hpp>
#include <vector>
#include "ConstantBuffer.h"
#include <filesystem>

typedef uint32_t GLuint;

namespace Cala {
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
		~Shader();
		Shader(const Shader& other) = delete;
		Shader(Shader&& other) noexcept;
		Shader& operator=(const Shader& other) = delete;
		Shader& operator=(Shader&& other) noexcept;
		void activate() const;
		void attachShader(ShaderType type, const std::filesystem::path& filePath);
		void createProgram();
		void dispatchComputeShader(uint32_t workGroupX = 1, uint32_t workGroupY = 1, uint32_t workGroupZ = 1) const;
		ConstantBuffer::ConstantBufferInfo getConstantBufferInfo(const std::string& bufferName) const;

		// Only neccessary when block binding point isn't explicitly defined in the shader code
		void attachConstantBuffer(ConstantBuffer* buffer, const std::string& bufferName) const;

	private:
		uint32_t attachedShaders = 0;

	#ifdef CALA_API_OPENGL
		GLuint programHandle = API_NULL;
		std::vector<GLuint> shaderHandlesBuffer;
	#endif
	};
}