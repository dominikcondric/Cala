#pragma once
#include "Core/Core.h"
#include "Core/Window.h"
#include "Graphics/GraphicsAPI.h"
#include "Core/Utility/Time.h"
#include "Graphics/RenderingSystem.h"

class BaseApplication {
public:
	BaseApplication(const Window::WindowSpecification& windowSpec, GraphicsAPI::API api);
	virtual ~BaseApplication() = default;
	void gameLoop();
	virtual void run() = 0;
	virtual void update() = 0;
	virtual void onWindowResize(const glm::ivec2& windowSize) = 0;
	virtual bool shouldExit();

protected:
	Unique<GraphicsAPI> renderer;
	Window mainWindow;
	Cala::Time time;
	RenderingSystem renderingSystem;
	float fpsLockValue = 60.f;
};

	

