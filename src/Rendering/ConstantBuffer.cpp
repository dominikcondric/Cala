#include "ConstantBuffer.h"
#include <glad/glad.h>
#include <cstring>
#include <glad/glad.h>
#include "Utility/Core.h"

namespace Cala {
#if CALA_API == CALA_API_OPENGL
	std::unordered_map<uint32_t, GLuint> ConstantBuffer::bindingPointsCache;
	ConstantBuffer::~ConstantBuffer()
	{
		glDeleteBuffers(1, &bufferID);
		bindingPointsCache.erase(specification.bindingPoint);
	}

	ConstantBuffer::ConstantBuffer(ConstantBuffer&& other) noexcept
	{
		*this = std::move(other);
	}

	ConstantBuffer& ConstantBuffer::operator=(ConstantBuffer&& other) noexcept
	{
		memcpy(this, &other, sizeof(ConstantBuffer));
		other.bufferID = GL_NONE;
		return *this;
	}

	void ConstantBuffer::setData(ConstantBufferInfo&& bufferInfo, bool isDynamic)
	{
		if (bufferInfo.size == -1)
		{
			LOG_ERROR("Buffer info is empty!");
			return;
		}

		if (bufferID != GL_NONE)
			glDeleteBuffers(1, &bufferID);

		specification = std::move(bufferInfo);

		if (bindingPointsCache.find(bufferInfo.bindingPoint) == bindingPointsCache.end())
		{
			glGenBuffers(1, &bufferID);
			glBindBuffer(GL_UNIFORM_BUFFER, bufferID);
			glBufferData(GL_UNIFORM_BUFFER, specification.size, nullptr, isDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
			glBindBufferBase(GL_UNIFORM_BUFFER, specification.bindingPoint, bufferID);
			glBindBuffer(GL_UNIFORM_BUFFER, GL_NONE);
			bindingPointsCache.insert({ specification.bindingPoint, bufferID });
		}
		else
		{
			bufferID = bindingPointsCache.at(bufferInfo.bindingPoint);
		}
	}

	void ConstantBuffer::updateData(const std::string& variableName, const void* data, uint32_t sizeInBytes) const
	{
		if (bufferID == GL_NONE)
		{
			LOG_ERROR("Buffer not initialized yet");
			return;
		}
		else if (specification.variablesInfo.find(variableName) == specification.variablesInfo.end())
		{
			LOG_ERROR("Variable name " << variableName << " doesn't exist");
			return;
		}

		glBindBuffer(GL_UNIFORM_BUFFER, bufferID);
		glBufferSubData(GL_UNIFORM_BUFFER, specification.variablesInfo.at(variableName).offset, sizeInBytes, data);
		glBindBuffer(GL_UNIFORM_BUFFER, GL_NONE);
	}
#else
	#error API is not supported
#endif
}