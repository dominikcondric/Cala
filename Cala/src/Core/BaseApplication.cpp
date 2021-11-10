#include "Graphics/OpenGL/OGLGraphicsAPI.h"
#include "BaseApplication.h"

BaseApplication::BaseApplication(const Window::WindowSpecification& windowSpec, Cala::RenderingAPI api) : mainWindow(windowSpec)
{
	switch (api)
	{
	case Cala::RenderingAPI::OpenGL:
		mainWindow.createOpenGLContext();
		renderer = Unique<OGLGraphicsAPI>(OGLGraphicsAPI::construct());
		break;
	case Cala::RenderingAPI::Direct3D:
		LOG("Direct3D not implemented yet. Loading OpenGL.");
		renderer = Unique<OGLGraphicsAPI>(OGLGraphicsAPI::construct());
		break;
	case Cala::RenderingAPI::Vulkan:
		LOG("Vulkan not implemented yet. Loading OpenGL.");
		renderer = Unique<OGLGraphicsAPI>(OGLGraphicsAPI::construct());
		break;
	}

	renderingSystem.initialize(renderer.get());
}

void BaseApplication::gameLoop()
{
	do {
		update();
		mainWindow.update();
		
		if (mainWindow.isResized())
			onWindowResize(mainWindow.getWindowSize());

		run();
		time.update();

		if (fpsLockValue >= 30.f && time.deltaTime < 1.f / fpsLockValue)
		{
			Cala::sleep(uint64_t(glm::floor((1.f / fpsLockValue - time.deltaTime) * 1000)) / 2);
			time.update();
		}

	} while (!shouldExit());
}

bool BaseApplication::shouldExit()
{
	return mainWindow.exitTriggered();
}
