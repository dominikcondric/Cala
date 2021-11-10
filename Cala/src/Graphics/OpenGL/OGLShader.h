#pragma once
#include "../Shader.h"

class OGLShader : public Shader {
public:
	OGLShader() = default;
	OGLShader(const OGLShader& other) = delete;
	OGLShader(OGLShader&& other) noexcept;
	~OGLShader() override;
	void attachShader(ShaderType shaderStage, const std::filesystem::path& filePath) override;
	void createProgram() override;
	void activate() const override;
	void dispatchComputeShader(uint32_t workGroupX, uint32_t workGroupY, uint32_t workGroupZ) const override;
	void attachConstantBuffer(ConstantBuffer* buffer, const std::string& bufferName) const override;
	ConstantBuffer::ConstantBufferInfo getConstantBufferInfo(const std::string& bufferName) const override;

private:
	GLuint programID = GL_NONE;
	std::vector<GLuint> shaderIDBuffer;
};

