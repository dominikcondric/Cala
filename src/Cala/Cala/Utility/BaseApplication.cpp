#include "BaseApplication.h"

#define MOVE_SPEED 5.f * time.getDeltaTime()

namespace Cala {

	BaseApplication::BaseApplication(const Window::Specification& windowSpecification) : 
		window(Window::construct(windowSpecification)), api(GraphicsAPI::construct()),
		camera(Camera::Type::Perspective)
	{
		camera.setProjectionAspectRatio((float)windowSpecification.width / windowSpecification.height);

		api->setBufferClearingColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.f));
		api->setBufferClearingBits(true, true, false);
		api->setViewport(glm::ivec4(0, 0, windowSpecification.width, windowSpecification.height));
	}

	BaseApplication::~BaseApplication()
	{
	}

    void BaseApplication::setFpsLock(float fps)
    {
		if (fps >= 30.f) {
			fpsLock = fps;
		} 
    }

    void BaseApplication::update()
    {
		if (moveCamera)
		{
			const IOSystem& ioSystem = window->getIO();

			if (ioSystem.isMouseButtonPressed(IOSystem::MOUSE_BUTTON_RIGHT))
				camera.rotate(ioSystem.getCursorOffset());

			if (ioSystem.isKeyPressed(IOSystem::KEY_W))
				camera.move(Camera::Directions::FORWARD, MOVE_SPEED);

			if (ioSystem.isKeyPressed(IOSystem::KEY_S))
				camera.move(Camera::Directions::BACKWARD, MOVE_SPEED);

			if (ioSystem.isKeyPressed(IOSystem::KEY_A))
				camera.move(Camera::Directions::LEFT, MOVE_SPEED);

			if (ioSystem.isKeyPressed(IOSystem::KEY_D))
				camera.move(Camera::Directions::RIGHT, MOVE_SPEED);

			if (ioSystem.isKeyPressed(IOSystem::KEY_SPACE))
				camera.move(Camera::Directions::UP, MOVE_SPEED);

			if (ioSystem.isKeyPressed(IOSystem::KEY_LEFT_SHIFT))
				camera.move(Camera::Directions::DOWN, MOVE_SPEED);
		}

		window->update();
		time.update();
		if (window->isResized())
		{
			auto winSize = window->getWindowSize();
			api->setViewport(glm::uvec4(0, 0, winSize.x, winSize.y));
			camera.setProjectionAspectRatio((float)winSize.x / winSize.y);
		}

		while (time.getFrameRate() > fpsLock) {
			time.incrementalUpdate();
		}
	}

	void BaseApplication::run()
	{
		while (!window->exitTriggered()) {
			loop();
			update();
		}
	}
}
