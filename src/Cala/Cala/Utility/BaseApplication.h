#pragma once

#include <Cala/Utility/Model.h>
#include <Cala/Rendering/Texture.h>
#include <Cala/Rendering/Renderers/LightRenderer.h>
#include <Cala/Rendering/Renderers/SimpleRenderer.h>
#include "Cala/Utility/Window.h"
#include <glm/gtc/random.hpp>
#include "Time.h"

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
		Time time;

		void setFpsLock(float fps);
		bool moveCamera = true;

	private:
		virtual void loop() = 0;
		void update();
		float fpsLock = 60.f;
	};
}