#pragma once
#include <glad/glad.h>
#include <string>
#include "../Texture.h"

class OGLTexture : public Texture {
public:
	OGLTexture() = default;
	~OGLTexture() override;
	OGLTexture(const OGLTexture& other) = delete;
	OGLTexture(OGLTexture&& other) noexcept;
	OGLTexture& operator=(OGLTexture&& other) noexcept;
	OGLTexture& operator=(const OGLTexture& other) = delete;
	void setForSampling(uint32_t bindingIndex) const override;
	void setForWriting(uint32_t bindingIndex) const override;
	void load2DTexture(const Image& image, const TextureSpecification& specification) override;
	void loadTexture(const void* data, uint32_t texWidth, uint32_t texHeight, Format format, Type texType, const TextureSpecification& specification) override;
	//void loadCubemap(const Image& image, const TextureSpecification& specification) override;
	void loadCubemap(const std::array<Image, 6>& images, const TextureSpecification& specification) override;
	const void* const getNativeTextureHandle() const { return (const void* const)textureID; }

private:
	GLuint textureID{ GL_NONE };
	uint32_t translateConstant(WrappingMethod method) const override;
	GLuint type;
	GLenum texInternalFormat;
};
