#include "ConstantBuffer.h"
#include <glad/glad.h>
#include <cstring>
#include "Cala/Utility/Logger.h"

namespace Cala {
#ifdef CALA_API_OPENGL
#include <glad/glad.h>
	std::unordered_map<uint32_t, GLuint> ConstantBuffer::bindingPointsCache;

	ConstantBuffer::~ConstantBuffer()
	{
		glDeleteBuffers(1, &bufferHandle);
		bindingPointsCache.erase(specification.bindingPoint);
	}

	ConstantBuffer::ConstantBuffer(ConstantBuffer&& other) noexcept
	{
		*this = std::move(other);
	}

	ConstantBuffer& ConstantBuffer::operator=(ConstantBuffer&& other) noexcept
	{
		other.bindingPointsCache = bindingPointsCache;
		other.errorOccured = errorOccured;
		other.specification = specification;
		other.bufferHandle = GL_NONE;
		return *this;
	}

	void ConstantBuffer::setData(ConstantBufferInfo&& bufferInfo, bool isDynamic)
	{
		if (bufferInfo.size == -1)
		{
			Logger::getInstance().logErrorToConsole("Buffer info is empty!");
			return;
		}

		if (bufferHandle != GL_NONE)
			glDeleteBuffers(1, &bufferHandle);

		specification = std::move(bufferInfo);

		if (bindingPointsCache.find(bufferInfo.bindingPoint) == bindingPointsCache.end())
		{
			errorOccured = false;
			glGenBuffers(1, &bufferHandle);
			glBindBuffer(GL_UNIFORM_BUFFER, bufferHandle);
			glBufferData(GL_UNIFORM_BUFFER, specification.size, nullptr, isDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
			glBindBufferBase(GL_UNIFORM_BUFFER, specification.bindingPoint, bufferHandle);
			glBindBuffer(GL_UNIFORM_BUFFER, GL_NONE);
			bindingPointsCache.insert({ specification.bindingPoint, bufferHandle });
		}
		else
		{
			bufferHandle = bindingPointsCache.at(bufferInfo.bindingPoint);
		}
	}

	void ConstantBuffer::updateData(const std::string& variableName, const void* data, uint32_t sizeInBytes) const
	{
		Logger& logger = Logger::getInstance();
		if (bufferHandle == GL_NONE)
		{
			if (!errorOccured)
				logger.logErrorToConsole("Buffer not initialized yet");

			errorOccured = true;
			return;
		}
		else if (specification.variablesInfo.find(variableName) == specification.variablesInfo.end())
		{
			logger.logErrorToConsole("Variable name " + variableName + " doesn't exist");
			return;
		}

		glBindBuffer(GL_UNIFORM_BUFFER, bufferHandle);
		glBufferSubData(GL_UNIFORM_BUFFER, specification.variablesInfo.at(variableName).offset, sizeInBytes, data);
		glBindBuffer(GL_UNIFORM_BUFFER, GL_NONE);
	}
#else
	#error API is not supported
#endif
}