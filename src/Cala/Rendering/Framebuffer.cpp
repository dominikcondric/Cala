#include "Framebuffer.h"
#include <iostream>
#include <cstring>
#include <glad/glad.h>

#define MAX_COLOR_ATTACHMENTS 32

namespace Cala {
#if CALA_API == CALA_API_OPENGL
	Framebuffer::~Framebuffer()
	{
		for (const auto& attachment : attachments)
		{
			switch (attachment.second)
			{
				case AttachmentType::Texture:
					glDeleteTextures(1, &attachment.first);
					break;
				case AttachmentType::Renderbuffer:
					glDeleteRenderbuffers(1, &attachment.first);
					break;
			}
		}
		glDeleteFramebuffers(1, &framebufferID);
	}

	Framebuffer::Framebuffer(Framebuffer&& other) noexcept
	{
		*this = std::move(other);
	}

	Framebuffer& Framebuffer::operator=(Framebuffer&& other) noexcept
	{
		memcpy(this, &other, sizeof(Framebuffer));
		other.framebufferID = GL_NONE;
		return *this;
	}

	void Framebuffer::generateShadowMap(uint32_t width, uint32_t height)
	{
		if (framebufferID != 0) 
			return;

		glGenFramebuffers(1, &framebufferID);
		activate();
		renderingTargetDimensions = { width, height };
		attachments.reserve(2);
		std::pair<GLuint, AttachmentType> depthStencilAttachment;
		glGenTextures(1, &depthStencilAttachment.first);
		glBindTexture(GL_TEXTURE_2D, depthStencilAttachment.first);
		depthStencilAttachment.second = AttachmentType::Texture;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[4]{ 1.f, 1.f, 1.f, 1.f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		attachments.emplace_back(std::move(depthStencilAttachment));

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthStencilAttachment.first, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindTexture(GL_TEXTURE_2D, GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
	}

	void Framebuffer::generateRenderingTarget(uint32_t width, uint32_t height, uint32_t colorAttachmentCount)
	{
		if (framebufferID != 0) 
			return;
		glGenFramebuffers(1, &framebufferID);

		activate();
		renderingTargetDimensions = { width, height };
		for (uint32_t i = 0; i < colorAttachmentCount; ++i)
		{
			std::pair<GLuint, AttachmentType> colorAttachment{ 0, AttachmentType::Texture };
			glGenTextures(1, &colorAttachment.first);
			glBindTexture(GL_TEXTURE_2D, colorAttachment.first);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorAttachment.first, 0);
			attachments.emplace_back(std::move(colorAttachment));

		}

		glBindTexture(GL_TEXTURE_2D, GL_NONE);
		if (colorAttachmentCount > 1)
		{
			uint32_t colorAttachmentEnum[MAX_COLOR_ATTACHMENTS];
			for (uint32_t i = 0; i < MAX_COLOR_ATTACHMENTS; ++i)
			{
				colorAttachmentEnum[i] = GL_COLOR_ATTACHMENT0 + i;
			}
			glDrawBuffers(colorAttachmentCount, colorAttachmentEnum);
		}

		std::pair<GLuint, AttachmentType> depthStencilAttachment;
		glGenRenderbuffers(1, &depthStencilAttachment.first);
		glBindRenderbuffer(GL_RENDERBUFFER, depthStencilAttachment.first);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthStencilAttachment.first);
		glBindRenderbuffer(GL_RENDERBUFFER, GL_NONE);
		attachments.emplace_back(depthStencilAttachment);

		// Check for errors
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER::Framebuffer is not complete!" << std::endl;

		glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
	}

	void Framebuffer::generateColorBuffer(uint32_t width, uint32_t height, uint32_t colorAttachmentCount)
	{
		if (framebufferID != 0) return;
		glGenFramebuffers(1, &framebufferID);
		activate();
		renderingTargetDimensions = { width, height };

		// Add color attachments
		for (uint32_t i = 0; i < colorAttachmentCount; ++i)
		{
			std::pair<GLuint, AttachmentType> colorAttachment{ 0, AttachmentType::Texture };
			glGenBuffers(1, &colorAttachment.first);
			glBindTexture(GL_TEXTURE_2D, colorAttachment.first);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorAttachment.first, 0);
			attachments.emplace_back(std::move(colorAttachment));
		}

		// Check for errors
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER::Framebuffer is not complete!" << std::endl;

		// unbind textures
		glBindTexture(GL_TEXTURE_2D, GL_NONE);

		if (colorAttachmentCount > 1)
		{
			uint32_t colorAttachmentEnum[MAX_COLOR_ATTACHMENTS];
			for (uint32_t i = 0; i < MAX_COLOR_ATTACHMENTS; ++i)
			{
				colorAttachmentEnum[i] = GL_COLOR_ATTACHMENT0 + i;
			}
			glDrawBuffers(colorAttachmentCount, colorAttachmentEnum);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
	}

	void Framebuffer::activate() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);
	}

	void Framebuffer::applyColorTexture(const uint32_t textureUnit, uint32_t attachmentIndex) const
	{
		glActiveTexture(textureUnit + GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, attachments[attachmentIndex].first);
	}
#else
	#error API not supported!
#endif
}