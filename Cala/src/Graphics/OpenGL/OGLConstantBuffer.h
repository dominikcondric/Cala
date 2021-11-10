#pragma once
#include "../ConstantBuffer.h"
#include <glad/glad.h>

class OGLConstantBuffer : public ConstantBuffer {
public:
	OGLConstantBuffer() = default;
	~OGLConstantBuffer() override;
	void setData(ConstantBufferInfo&& bufferInfo, bool isDynamic = true) override;
	void updateData(const std::string& variableName, const void* data, uint32_t sizeInBytes) override;
	
private:
	GLuint ubo{ GL_NONE };
};

