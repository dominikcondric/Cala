#include <glad/glad.h>
#include <sstream>
#include <fstream>
#include <string>
#include <iostream>
#include "Shader.h"
#include <cstring>
#include <glad/glad.h>
#include "Utility/Core.h"

#define BIT(x) (1 << x)

#if CALA_API == CALA_API_OPENGL
	Shader::Shader(Shader&& other) noexcept
	{
		*this = std::move(other);
	}

	Shader& Shader::operator=(Shader&& other) noexcept
	{
		memcpy(this, &other, sizeof(Shader));
		other.programID = GL_NONE;
		return *this;
	}

	Shader::~Shader()
	{
		glDeleteProgram(programID);
	}

	void Shader::attachShader(ShaderType shaderStage, const std::filesystem::path& filePath)
	{
		if (programID != GL_NONE)
		{
			attachedShaders = 0;
			glDeleteProgram(programID);
		}

		if (attachedShaders & BIT((uint32_t)shaderStage))
		{
			LOG("Error: shader stage already attached!");
			return;
		}

		if ((attachedShaders && shaderStage == ShaderType::ComputeShader && !(attachedShaders & BIT((uint32_t)ShaderType::ComputeShader)))
			|| ((attachedShaders & BIT((uint32_t)ShaderType::ComputeShader)) && shaderStage != ShaderType::ComputeShader))
		{
			LOG("Error: Can't mix compute and drawing shaders!");
			return;
		}

		std::string shaderCode;
		std::ifstream shaderFile;

		shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try {
			shaderFile.open(filePath);
			std::stringstream shaderStream;
			shaderStream << shaderFile.rdbuf();
			shaderCode = shaderStream.str();
			shaderFile.close();
		}
		catch (std::ifstream::failure& e) {
			std::cout << e.what() << std::endl;
			std::cout << "Error: Cannot find shader file(" << filePath.string() << ")!" << std::endl;
		}

		GLint success;
		char infoLog[512];
		const char* shaderCodeStringLiteral = shaderCode.c_str();
		GLenum shaderType = 0;

		switch (shaderStage)
		{
			case ShaderType::VertexShader:				shaderType = GL_VERTEX_SHADER; break;
			case ShaderType::FragmentShader:			shaderType = GL_FRAGMENT_SHADER; break;
			case ShaderType::GeometryShader:			shaderType = GL_GEOMETRY_SHADER; break;
			case ShaderType::ComputeShader:				shaderType = GL_COMPUTE_SHADER; break;
		}

		GLuint shaderID = glCreateShader(shaderType);
		glShaderSource(shaderID, 1, &shaderCodeStringLiteral, nullptr);
		glCompileShader(shaderID);
		glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shaderID, 512, nullptr, infoLog);
			std::cout << "Failed to compile shader: " << filePath << " " << infoLog << std::endl;
		}

		attachedShaders |= BIT((uint32_t)shaderStage);
		shaderIDBuffer.push_back(shaderID);
	}

	void Shader::createProgram()
	{
		if (programID)
		{
			LOG("Error: Program already created!");
			return;
		}

		char infoLog[512];
		GLint success;
		programID = glCreateProgram();
		for (const auto& shader : shaderIDBuffer)
		{
			glAttachShader(programID, shader);
		}

		glLinkProgram(programID);
		glGetProgramiv(programID, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(programID, 512, nullptr, infoLog);
			std::cout << "Failed to link program: " << infoLog << std::endl;
		}

		for (const auto& shader : shaderIDBuffer)
		{
			glDetachShader(programID, shader);
			glDeleteShader(shader);
		}
	}

	void Shader::activate() const
	{
		glUseProgram(programID);
	}

	void Shader::dispatchComputeShader(uint32_t workGroupX, uint32_t workGroupY, uint32_t workGroupZ) const
	{
		if (!(attachedShaders & BIT((uint32_t)ShaderType::ComputeShader)))
			return;

		glDispatchCompute(workGroupX, workGroupY, workGroupZ);
	}

	void Shader::attachConstantBuffer(ConstantBuffer* buffer, const std::string& bufferName) const
	{
		if (programID == GL_NONE)
			return;

		int blockIndex = glGetUniformBlockIndex(programID, bufferName.c_str());

		if (blockIndex == GL_INVALID_INDEX)
		{
			 LOG("Invalid block index");
			return;
		}

		glUniformBlockBinding(programID, blockIndex, buffer->getBindingPoint());
	}

	ConstantBuffer::ConstantBufferInfo Shader::getConstantBufferInfo(const std::string& bufferName) const
	{
		ConstantBuffer::ConstantBufferInfo bufferInfo;
		GLint blockIndex = glGetUniformBlockIndex(programID, bufferName.c_str());
		if (blockIndex == -1)
			return bufferInfo;

		glGetActiveUniformBlockiv(programID, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &bufferInfo.size);
		GLint indices[1000];
		GLint indicesCount = 0;
		glGetActiveUniformBlockiv(programID, blockIndex, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &indicesCount);
		glGetActiveUniformBlockiv(programID, blockIndex, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, indices);
		glGetActiveUniformBlockiv(programID, blockIndex, GL_UNIFORM_BLOCK_BINDING, &bufferInfo.bindingPoint);

		bufferInfo.variablesInfo.reserve(indicesCount);
		for (int i = 0; i < indicesCount; ++i)
		{
			ConstantBuffer::ConstantBufferVariableInfo variableInfo;
			glGetActiveUniformsiv(programID, 1, (GLuint*)&indices[i], GL_UNIFORM_OFFSET, &variableInfo.offset);
			glGetActiveUniformsiv(programID, 1, (GLuint*)&indices[i], GL_UNIFORM_ARRAY_STRIDE, &variableInfo.arrayStride);
			glGetActiveUniformsiv(programID, 1, (GLuint*)&indices[i], GL_UNIFORM_MATRIX_STRIDE, &variableInfo.matrixStride);
			variableInfo.indexInBlock = indices[i];
			GLchar name[50];
			glGetActiveUniformName(programID, indices[i], 50, nullptr, name);
			bufferInfo.variablesInfo.insert(std::make_pair(std::string(name), variableInfo));
		}

		return bufferInfo;
	}
#else
	#error API is not supported!
#endif