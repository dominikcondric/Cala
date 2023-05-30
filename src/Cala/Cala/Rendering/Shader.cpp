#include <glad/glad.h>
#include <sstream>
#include <fstream>
#include <string>
#include <iostream>
#include "Shader.h"
#include <cstring>
#include "Cala/Utility/Logger.h"

#define BIT(x) (1 << x)

namespace Cala {
#ifdef CALA_API_OPENGL
#include <glad/glad.h>
	Shader::Shader(Shader&& other) noexcept
	{
		*this = std::move(other);
	}

	Shader& Shader::operator=(Shader&& other) noexcept
	{
		programHandle = other.programHandle;
		other.programHandle = GL_NONE;
		attachedShaders = other.attachedShaders;
		shaderHandlesBuffer = other.shaderHandlesBuffer;
		return *this;
	}

    Shader::~Shader()
	{
		free();
	}

    void Shader::free()
    {
		if (attachedShaders != 0)
		{
			for (const auto& shader : shaderHandlesBuffer)
			{
				glDetachShader(programHandle, shader);
				glDeleteShader(shader);
			}
		}

		glDeleteProgram(programHandle);
    }

    bool Shader::isLoaded() const
    {
        return programHandle != API_NULL;
    }

    void Shader::attachShader(ShaderType shaderStage, const std::filesystem::path& filePath)
	{
		Logger& logger = Logger::getInstance();

		if (isLoaded())
		{
			logger.logErrorToConsole("Shader program already created!");
			return;
		}

		if (attachedShaders & BIT((uint32_t)shaderStage))
		{
			logger.logErrorToConsole("Shader stage already attached!");
			return;
		}

		if ((attachedShaders && shaderStage == ShaderType::ComputeShader && !(attachedShaders & BIT((uint32_t)ShaderType::ComputeShader)))
			|| ((attachedShaders & BIT((uint32_t)ShaderType::ComputeShader)) && shaderStage != ShaderType::ComputeShader))
		{
			logger.logErrorToConsole("Error: Can't mix compute and drawing shaders!");
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
		shaderHandlesBuffer.push_back(shaderID);
	}

	void Shader::createProgram()
	{
		if (programHandle)
		{
			Logger::getInstance().logErrorToConsole("Program already created!");
			return;
		}

		char infoLog[512];
		GLint success;
		programHandle = glCreateProgram();
		for (const auto& shader : shaderHandlesBuffer)
		{
			glAttachShader(programHandle, shader);
		}

		glLinkProgram(programHandle);
		glGetProgramiv(programHandle, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(programHandle, 512, nullptr, infoLog);
			Logger::getInstance().logErrorToConsole(std::string("Failed to link program: ") + infoLog);
		}

		for (const auto& shader : shaderHandlesBuffer)
		{
			glDetachShader(programHandle, shader);
			glDeleteShader(shader);
		}

		attachedShaders = 0;
		shaderHandlesBuffer.clear();
	}

	void Shader::activate() const
	{
		glUseProgram(programHandle);
	}

	void Shader::dispatchComputeShader(uint32_t workGroupX, uint32_t workGroupY, uint32_t workGroupZ) const
	{
		if (!(attachedShaders & BIT((uint32_t)ShaderType::ComputeShader)))
			return;

		glDispatchCompute(workGroupX, workGroupY, workGroupZ);
	}

	void Shader::attachConstantBuffer(ConstantBuffer* buffer, const std::string& bufferName) const
	{
		if (programHandle == GL_NONE)
			return;

		uint32_t blockIndex = glGetUniformBlockIndex(programHandle, bufferName.c_str());

		if (blockIndex == GL_INVALID_INDEX)
		{
			Logger::getInstance().logErrorToConsole("Invalid block index");
			return;
		}

		glUniformBlockBinding(programHandle, blockIndex, buffer->getBindingPoint());
	}

	ConstantBuffer::ConstantBufferInfo Shader::getConstantBufferInfo(const std::string& bufferName) const
	{
		ConstantBuffer::ConstantBufferInfo bufferInfo;
		GLint blockIndex = glGetUniformBlockIndex(programHandle, bufferName.c_str());
		if (blockIndex == -1)
			return bufferInfo;

		glGetActiveUniformBlockiv(programHandle, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &bufferInfo.size);
		GLint indices[1000];
		GLint indicesCount = 0;
		glGetActiveUniformBlockiv(programHandle, blockIndex, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &indicesCount);
		glGetActiveUniformBlockiv(programHandle, blockIndex, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, indices);
		glGetActiveUniformBlockiv(programHandle, blockIndex, GL_UNIFORM_BLOCK_BINDING, &bufferInfo.bindingPoint);

		bufferInfo.variablesInfo.reserve(indicesCount);
		for (int i = 0; i < indicesCount; ++i)
		{
			ConstantBuffer::ConstantBufferVariableInfo variableInfo;
			glGetActiveUniformsiv(programHandle, 1, (GLuint*)&indices[i], GL_UNIFORM_OFFSET, &variableInfo.offset);
			glGetActiveUniformsiv(programHandle, 1, (GLuint*)&indices[i], GL_UNIFORM_ARRAY_STRIDE, &variableInfo.arrayStride);
			glGetActiveUniformsiv(programHandle, 1, (GLuint*)&indices[i], GL_UNIFORM_MATRIX_STRIDE, &variableInfo.matrixStride);
			variableInfo.indexInBlock = indices[i];
			GLchar name[50];
			glGetActiveUniformName(programHandle, indices[i], 50, nullptr, name);
			bufferInfo.variablesInfo.insert(std::make_pair(std::string(name), variableInfo));
		}

		return bufferInfo;
	}
#else
	#error API is not supported!
#endif
}