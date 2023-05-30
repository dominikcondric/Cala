#include "Framebuffer.h"
#include <iostream>
#include <cstring>
#include "Cala/Utility/Logger.h"

#define MAX_COLOR_ATTACHMENTS 32

namespace Cala {
#ifdef CALA_API_OPENGL
#include <glad/glad.h>

	Framebuffer::~Framebuffer()
	{
		free();
	}

	Framebuffer::Framebuffer(Framebuffer&& other) noexcept
	{
		*this = std::move(other);
	}

	Framebuffer& Framebuffer::operator=(Framebuffer&& other) noexcept
	{
		colorTargets = std::move(other.colorTargets);
		depthTarget = std::move(other.depthTarget);
		framebufferHandle = other.framebufferHandle;
		other.framebufferHandle = GL_NONE;
		return *this;
	}

    void Framebuffer::addColorTarget(Texture* target, bool transferOwnership)
    {
		if (!target->isColor() || colorTargets.size() == MAX_COLOR_ATTACHMENTS)
			return;

		colorTargets.emplace_back(target, transferOwnership, 0U);
    }

    void Framebuffer::addDepthTarget(Texture* target, bool transferOwnership)
    {
		if (depthTarget.target != nullptr || target->isColor())
			return;

		depthTarget.target = target;
		depthTarget.owned = transferOwnership;
		depthTarget.layer = 0U;
    }

    void Framebuffer::free()
    {
		for (auto& target : colorTargets)
			target->free();
		
		depthTarget->free();
		glDeleteFramebuffers(1, &framebufferHandle);
		framebufferHandle = GL_NONE;
    }

    void Framebuffer::addColorTarget(TextureArray *target, bool transferOwnership, uint32_t layer)
    {
		if (!target->isColor() || colorTargets.size() == MAX_COLOR_ATTACHMENTS || layer >= target->getLayerCount())
			return;

		colorTargets.emplace_back(target, transferOwnership, layer);
    }

    void Framebuffer::addDepthTarget(TextureArray *target, bool transferOwnership, uint32_t layer)
    {
		if (depthTarget.target != nullptr || target->isColor() || layer >= target->getLayerCount())
			return;

		depthTarget.target = target;
		depthTarget.owned = transferOwnership;
		depthTarget.layer = layer;
    }

    bool Framebuffer::isLoaded() const
    {
        return framebufferHandle != API_NULL;
    }

    void Framebuffer::load()
    {
		if (colorTargets.empty() && depthTarget.target == nullptr)
			return;

		if (isLoaded())
		{
			Logger::getInstance().logErrorToConsole("Framebuffer already loaded!");
			return;
		}
		
		glGenFramebuffers(1, &framebufferHandle);

		glBindFramebuffer(GL_FRAMEBUFFER, framebufferHandle);

		if (!colorTargets.empty()) // If there are color targets
		{
			uint32_t colorAttachmentEnum[MAX_COLOR_ATTACHMENTS];

			for (uint32_t i = 0; i < (uint32_t)colorTargets.size(); ++i)
			{
				const ITexture& texture = *colorTargets[i].target;
				colorAttachmentEnum[i] = GL_COLOR_ATTACHMENT0 + i;
				GLuint textureHandle = texture.getNativeHandle();
				if (texture.isWriteOnly())
				{
					glFramebufferRenderbuffer(GL_FRAMEBUFFER, colorAttachmentEnum[i], GL_RENDERBUFFER, textureHandle);
				}
				else
				{
					if (colorTargets[i].layer == 0)
					{
						glFramebufferTexture(GL_FRAMEBUFFER, colorAttachmentEnum[i], textureHandle, 0);
					}
					else
					{
						glFramebufferTextureLayer(GL_FRAMEBUFFER, colorAttachmentEnum[i], textureHandle, 0, colorTargets[i].layer);
					} 
				}
			}
			glDrawBuffers((GLsizei)colorTargets.size(), colorAttachmentEnum);
		} 
		else
		{
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
		}

		if (depthTarget.target != nullptr)
		{	
			const ITexture& depthTexture = *depthTarget.target;
			GLuint textureHandle = depthTexture.getNativeHandle();
			GLenum attachmentType = depthTarget->isDepth() ? GL_DEPTH_ATTACHMENT : GL_DEPTH_STENCIL_ATTACHMENT;
			if (depthTexture.isWriteOnly())
			{
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachmentType, GL_RENDERBUFFER, textureHandle);
			}
			else
			{
				if (depthTarget.layer == 0)
				{
					glFramebufferTexture(GL_FRAMEBUFFER, attachmentType, textureHandle, 0);
				}
				else
				{
					glFramebufferTextureLayer(GL_FRAMEBUFFER, attachmentType, textureHandle, 0, depthTarget.layer);
				}
			}
		}

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			Logger::getInstance().logErrorToConsole("Framebuffer is not complete!");
		}

		glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
    }

    const ITexture& Framebuffer::getColorTarget(uint32_t index) const
	{
		assert(index < colorTargets.size());
		return *colorTargets[index].target;
	}
    const ITexture &Framebuffer::getDepthTarget() const
    {
		assert(depthTarget.target != nullptr);
		return *depthTarget.target;
    }

#else
#error API not supported!
#endif

    Framebuffer::Target::Target(ITexture *_target, bool _owned, uint32_t _layer) : target(_target), owned(_owned), layer(_layer)
    {
    }

    Framebuffer::Target::Target(Target &&other) noexcept
    {
		*this = std::move(other);
    }

    Framebuffer::Target& Framebuffer::Target::operator=(Target&& other) noexcept
    {
		target = other.target;
		owned = other.owned;
		layer = other.layer;
		other.owned = false;
		return *this;
    }

    Framebuffer::Target::~Target()
    {
		if (owned)
			delete target;
    }

    ITexture* Framebuffer::Target::operator->()
    {
        return target;
    }
}