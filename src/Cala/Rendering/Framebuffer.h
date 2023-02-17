#pragma once
#include <stdint.h>
#include <glm/vec2.hpp>
#include <vector>

typedef uint32_t GLuint;

namespace Cala {
	class Framebuffer {
	public:
		Framebuffer() = default;
		~Framebuffer();
		Framebuffer(const Framebuffer& other) = delete;
		Framebuffer(Framebuffer&& other) noexcept;
		Framebuffer& operator=(const Framebuffer& other) = delete;
		Framebuffer& operator=(Framebuffer&& other) noexcept;
		void generateShadowMap(uint32_t width, uint32_t height);
		void generateRenderingTarget(uint32_t width, uint32_t height, uint32_t colorAttachmentCount);
		void generateColorBuffer(uint32_t width, uint32_t height, uint32_t colorAttachmentCount);
		void activate() const;
		//void applyDepthTexture(const uint32_t textureUnit) const;
		void applyColorTexture(const uint32_t textureUnit, uint32_t attachmentIndex) const;
		const glm::uvec2& getRenderingTargetDimensions() const { return renderingTargetDimensions; }

	protected:
		glm::uvec2 renderingTargetDimensions;
	#if CALA_API == CALA_API_OPENGL
		enum class AttachmentType { Texture, Renderbuffer };
		GLuint framebufferID = 0;
		std::vector<std::pair<GLuint, AttachmentType>> attachments;
	#else
		#error Api not supported!
	#endif 
	};
}