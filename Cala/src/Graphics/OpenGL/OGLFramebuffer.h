#pragma once
#include "../Framebuffer.h"
#include "OGLTexture.h"
	
class OGLFramebuffer : public Framebuffer {
public:
	OGLFramebuffer() = default;
	~OGLFramebuffer();
	void generateShadowMap(uint32_t width, uint32_t height) override;
	void generateRenderingTarget(uint32_t width, uint32_t height, uint32_t colorAttachmentCount) override;
	void generateColorBuffer(uint32_t width, uint32_t height, uint32_t colorAttachmentCount) override;
	void activate() const override;
	void applyColorTexture(const uint32_t textureUnit, uint32_t attachmentIndex) const override;

private:
	enum class AttachmentType { Texture, Renderbuffer };
	GLuint framebufferID = GL_NONE;
	std::vector<std::pair<GLuint, AttachmentType>> attachments;
};
