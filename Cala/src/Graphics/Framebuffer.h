#pragma once
#include <stdint.h>
#include <glm/vec2.hpp>
#include <vector>

class Framebuffer {
public:
	Framebuffer() = default;
	virtual ~Framebuffer() = default;
	virtual void generateShadowMap(uint32_t width, uint32_t height) = 0;
	virtual void generateRenderingTarget(uint32_t width, uint32_t height, uint32_t colorAttachmentCount) = 0;
	virtual void generateColorBuffer(uint32_t width, uint32_t height, uint32_t colorAttachmentCount) = 0;
	virtual void activate() const = 0;
	//virtual void applyDepthTexture(const uint32_t textureUnit) const = 0;
	virtual void applyColorTexture(const uint32_t textureUnit, uint32_t attachmentIndex) const = 0;
	const glm::uvec2& getRenderingTargetDimensions() const { return renderingTargetDimensions; }

protected:
	glm::uvec2 renderingTargetDimensions;
};