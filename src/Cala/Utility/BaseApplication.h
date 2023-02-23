#pragma once

#include <Utility/Model.h>
#include <Rendering/Texture.h>
#include <Rendering/Renderers/LightRenderer.h>
#include <Rendering/Renderers/SimpleRenderer.h>
#include "Utility/Window.h"
#include <glm/gtc/random.hpp>

#define MOVE_SPEED 5.f * deltaTime

namespace Cala {
	class BaseApplication {
	public:
		BaseApplication(const Window::Specification& windowSpecification);
		virtual ~BaseApplication() = 0;
		void run();

	protected:
		Unique<Window> window;
		Unique<GraphicsAPI> api;
		PerspectiveCamera camera;

	private:
		virtual void loop() = 0;
		void update();
	};
}