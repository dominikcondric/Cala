#pragma once
#include <stdint.h>
#include <glm/vec2.hpp>
#include <vector>
#include "Texture.h"
#include "TextureArray.h"
#include "NativeAPI.h"

namespace Cala {
	class Framebuffer {
	public:
		Framebuffer() = default;
		~Framebuffer();
		Framebuffer(const Framebuffer& other) = delete;
		Framebuffer(Framebuffer&& other) noexcept;
		Framebuffer& operator=(const Framebuffer& other) = delete;
		Framebuffer& operator=(Framebuffer&& other) noexcept;
		void addColorTarget(Texture* target, bool transferOwnership);
		void addDepthTarget(Texture* target, bool transferOwnership);

		/**
		 * Adds color target to a framebuffer
		 * If layer = 0, texture is bound to the framebuffer for layered rendering
		*/
		void addColorTarget(TextureArray* target,  bool transferOwnership, uint32_t layer);

		/**
		 * Adds depth target to a framebuffer
		 * * If layer = 0, texture is bound to the framebuffer for layered rendering
		*/
		void addDepthTarget(TextureArray* target, bool transferOwnership, uint32_t layer);

		void load();
        const ITexture& getColorTarget(uint32_t index) const;
		const ITexture& getDepthTarget() const;

	private:
		struct Target {
			Target(ITexture* _target = nullptr, bool _owned = false, uint32_t _layer = 0);
			Target(const Target& other) = delete;
			Target(Target&& other) noexcept;
			Target& operator=(const Target& other) = delete;
			Target& operator=(Target&& other) noexcept;
			~Target();
			ITexture* operator->();

			ITexture* target = nullptr;
			bool owned = false;
			uint32_t layer;
		};

		std::vector<Target> colorTargets;
		Target depthTarget;

	// API specific
	#ifdef CALA_API_OPENGL
	public:
		GLuint getNativeFramebufferHandle() const { return framebufferHandle; }
	private:
		GLuint framebufferHandle = API_NULL;
	#endif 
	};
}