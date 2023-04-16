#pragma once 
#include "Cala/Rendering/GraphicsAPI.h"
#include "Cala/Rendering/Camera.h"

namespace Cala {
	class IRenderer {
	public:
		IRenderer() = default;
		virtual ~IRenderer() = default;
		virtual void render(GraphicsAPI* const api, const Framebuffer* renderingTarget) = 0;
	};
}