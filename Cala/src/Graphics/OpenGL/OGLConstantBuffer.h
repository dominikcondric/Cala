#pragma once
#include "../ConstantBuffer.h"
#include <glad/glad.h>

class OGLConstantBuffer : public ConstantBuffer {
public:
	OGLConstantBuffer() = default;
	~OGLConstantBuffer() override;
	OGLConstantBuffer(const OGLConstantBuffer& other) = delete;
	OGLConstantBuffer(OGLConstantBuffer&& other) noexcept;
	OGLConstantBuffer& operator=(OGLConstantBuffer&& other) noexcept;
	OGLConstantBuffer& operator=(const OGLConstantBuffer& other) = delete;
	void setData(ConstantBufferInfo&& bufferInfo, bool isDynamic = true) override;
	void updateData(const std::string& variableName, const void* data, uint32_t sizeInBytes) override;
	
private:
	GLuint bufferID{ GL_NONE };
};

