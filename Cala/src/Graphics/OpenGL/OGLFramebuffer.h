#pragma once
#include "../Framebuffer.h"
#include <glad/glad.h>
	
class OGLFramebuffer : public Framebuffer {
public:
	OGLFramebuffer() = default;
	~OGLFramebuffer();
	OGLFramebuffer(const OGLFramebuffer& other) = delete;
	OGLFramebuffer(OGLFramebuffer&& other) noexcept;
	OGLFramebuffer& operator=(OGLFramebuffer&& other) noexcept;
	OGLFramebuffer& operator=(const OGLFramebuffer& other) = delete;
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
