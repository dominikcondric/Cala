#include "IIOSystem.h"
#include <GLFW/glfw3.h>

namespace Cala {
	IIOSystem* IIOSystem::instance = nullptr;

	IIOSystem::IIOSystem()
	{
		assert(!instance);
		instance = this;
	}

    IIOSystem::~IIOSystem()
    {
		instance = nullptr;
    }

    void IIOSystem::update()
    {
		lastX = cursorX;
		lastY = cursorY;
		auto cursorPosition = apiGetCursorPosition();
		cursorX = cursorPosition.x;
		cursorY = cursorPosition.y;

		for (uint32_t i = 0; i < oldKeyStates.size(); ++i)
		{
			oldKeyStates[i] = newKeyStates[i];
			if (i < 3)
				newKeyStates[i] = apiIsMouseButtonPressed((KeyCode)i);
			else
				newKeyStates[i] = apiIsKeyPressed((KeyCode)i);
		}
	}

	bool IIOSystem::isKeyPressed(KeyCode key) const
	{
		return newKeyStates[key];
	}

	bool IIOSystem::isKeyTapped(KeyCode key) const
	{
		return newKeyStates[key] && !oldKeyStates[key];
	}

	bool IIOSystem::isMouseButtonPressed(KeyCode button) const
	{
		if (button > MOUSE_BUTTON_MIDDLE)
			return false;

		return isKeyPressed(button);
	}

	bool IIOSystem::isMouseButtonTapped(KeyCode button) const
	{
		if (button > MOUSE_BUTTON_MIDDLE)
			return false;

		return isKeyTapped(button);
	}

	glm::vec2 IIOSystem::getCursorPosition() const
	{
		return glm::vec2(cursorX, cursorY);
	}

	glm::vec2 IIOSystem::getCursorOffset() const
	{
		glm::vec2 cursorOff;
		cursorOff.x = (float)cursorX - (float)lastX;
		cursorOff.y = (float)lastY - (float)cursorY;
		return cursorOff;
	}

}