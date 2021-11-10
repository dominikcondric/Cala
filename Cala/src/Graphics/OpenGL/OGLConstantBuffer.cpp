#include "OGLConstantBuffer.h"
#include "Core/Core.h"

OGLConstantBuffer::~OGLConstantBuffer()
{
	glDeleteBuffers(1, &ubo);
}

void OGLConstantBuffer::setData(ConstantBufferInfo&& bufferInfo, bool isDynamic)
{
	if (bufferInfo.size == -1)
	{
		LOG_ERROR("Buffer info is empty!");
		return;
	}

	if (ubo != GL_NONE)
		glDeleteBuffers(1, &ubo);

	specification = std::move(bufferInfo);
	glGenBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferData(GL_UNIFORM_BUFFER, specification.size, nullptr, isDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, specification.bindingPoint, ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, GL_NONE);
}

void OGLConstantBuffer::updateData(const std::string& variableName, const void* data, uint32_t sizeInBytes)
{
	if (ubo == GL_NONE)
	{
		LOG_ERROR("Buffer not initialized yet");
		return;
	}
	else if (specification.variablesInfo.find(variableName) == specification.variablesInfo.end())
	{
		LOG_ERROR("Variable name " << variableName << " doesn't exist");
		return;
	}

	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, specification.variablesInfo[variableName].offset, sizeInBytes, data);
	glBindBuffer(GL_UNIFORM_BUFFER, GL_NONE);
}