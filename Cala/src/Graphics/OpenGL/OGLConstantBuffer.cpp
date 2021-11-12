#include "OGLConstantBuffer.h"
#include "Core/Core.h"
#include <cstring>

OGLConstantBuffer::~OGLConstantBuffer()
{
	glDeleteBuffers(1, &bufferID);
}

OGLConstantBuffer::OGLConstantBuffer(OGLConstantBuffer&& other) noexcept
{
	*this = std::move(other);
}

OGLConstantBuffer& OGLConstantBuffer::operator=(OGLConstantBuffer&& other) noexcept
{
	memcpy(this, &other, sizeof(OGLConstantBuffer));
	other.bufferID = GL_NONE;
	return *this;
}

void OGLConstantBuffer::setData(ConstantBufferInfo&& bufferInfo, bool isDynamic)
{
	if (bufferInfo.size == -1)
	{
		LOG_ERROR("Buffer info is empty!");
		return;
	}

	if (bufferID != GL_NONE)
		glDeleteBuffers(1, &bufferID);

	specification = std::move(bufferInfo);
	glGenBuffers(1, &bufferID);
	glBindBuffer(GL_UNIFORM_BUFFER, bufferID);
	glBufferData(GL_UNIFORM_BUFFER, specification.size, nullptr, isDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, specification.bindingPoint, bufferID);
	glBindBuffer(GL_UNIFORM_BUFFER, GL_NONE);
}

void OGLConstantBuffer::updateData(const std::string& variableName, const void* data, uint32_t sizeInBytes)
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
	glBufferSubData(GL_UNIFORM_BUFFER, specification.variablesInfo[variableName].offset, sizeInBytes, data);
	glBindBuffer(GL_UNIFORM_BUFFER, GL_NONE);
}