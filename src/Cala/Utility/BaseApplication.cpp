#include "BaseApplication.h"

namespace Cala {

	BaseApplication::BaseApplication(const Window::Specification& windowSpecification) : 
		window(Window::construct(windowSpecification)), api(GraphicsAPI::construct())
	{
		camera.setProjectionAspectRatio((float)windowSpecification.width / windowSpecification.height);

		api->setBufferClearingColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.f));
		api->setBufferClearingBits(true, true, false);
		api->setViewport(glm::ivec4(0, 0, windowSpecification.width, windowSpecification.height));
	}

	void BaseApplication::update()
	{
		const IOSystem& ioSystem = window->getIO();
		const float deltaTime = window->getTime().deltaTime;

		if (ioSystem.isMouseButtonPressed(IOSystem::MOUSE_BUTTON_RIGHT))
			camera.rotateCamera(ioSystem.getCursorOffset());

		if (ioSystem.isKeyPressed(IOSystem::KEY_W))
			camera.moveCamera(Camera::Directions::FORWARD, MOVE_SPEED);

		if (ioSystem.isKeyPressed(IOSystem::KEY_S))
			camera.moveCamera(Camera::Directions::BACKWARD, MOVE_SPEED);

		if (ioSystem.isKeyPressed(IOSystem::KEY_A))
			camera.moveCamera(Camera::Directions::LEFT, MOVE_SPEED);

		if (ioSystem.isKeyPressed(IOSystem::KEY_D))
			camera.moveCamera(Camera::Directions::RIGHT, MOVE_SPEED);

		if (ioSystem.isKeyPressed(IOSystem::KEY_SPACE))
			camera.moveCamera(Camera::Directions::UP, MOVE_SPEED);

		if (ioSystem.isKeyPressed(IOSystem::KEY_LEFT_SHIFT))
			camera.moveCamera(Camera::Directions::DOWN, MOVE_SPEED);

		window->update();
		if (window->isResized())
		{
			auto winSize = window->getWindowSize();
			api->setViewport(glm::uvec4(0, 0, winSize.x, winSize.y));
			camera.setProjectionAspectRatio((float)winSize.x / winSize.y);
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
