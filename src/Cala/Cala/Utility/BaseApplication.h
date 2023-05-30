#pragma once
#include "Cala/Utility/GLFWWindow.h"
#include "Time.h"
#include "Cala/Rendering/Camera.h"
#include "Cala/Rendering/GraphicsAPI.h"

namespace Cala {
	class BaseApplication {
	public:
		BaseApplication(const IWindow::Specification& windowSpecification);
		virtual ~BaseApplication() = 0;
		void run();

	protected:
		std::unique_ptr<IWindow> window;
		std::unique_ptr<GraphicsAPI> api;
		Camera camera;
		Time time;

		void setFpsLock(float fps);
		bool moveCamera = true;

	private:
		virtual void loop() = 0;
		void update();
		float fpsLock = 60.f;
	};
}