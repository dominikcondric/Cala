#include "IOSystem.h"
#include <GLFW/glfw3.h>

namespace Cala {
	IOSystem* IOSystem::instance = nullptr;

	IOSystem::IOSystem(GLFWwindow* windowPointer) : windowHandle(windowPointer)
	{
		keyStates.reserve(100);
		for (uint32_t i = 0; i < 100; ++i)
			keyStates.push_back(false);
	}

    IOSystem *IOSystem::construct(GLFWwindow* windowPointer)
    {
        if (!instance)
			return instance = new IOSystem(windowPointer);

		return instance;
    }

    IOSystem::~IOSystem()
    {
		instance = nullptr;
    }

    void IOSystem::update()
    {
		lastX = cursorX;
		lastY = cursorY;
		glfwGetCursorPos(windowHandle, &cursorX, &cursorY);

		for (uint32_t i = 0; i < keyStates.size(); ++i)
			keyStates[i] = isKeyPressed((KeyCode)i);

		glfwPollEvents();
	}

	bool IOSystem::isKeyPressed(KeyCode key) const
	{
		return bool(glfwGetKey(windowHandle, mapEngineToLibraryKey(key)));
	}

	bool IOSystem::isKeyTapped(KeyCode key) const
	{
		return (isKeyPressed(key) && !keyStates[(int)key]);
	}

	bool IOSystem::isMouseButtonPressed(KeyCode button) const
	{
		return bool(glfwGetMouseButton(windowHandle, mapEngineToLibraryKey(button)));
	}

	bool IOSystem::isMouseButtonTapped(KeyCode button) const
	{
		return bool(isMouseButtonPressed(button) && !keyStates[(int)button]);
	}

	glm::vec2 IOSystem::getCursorPosition() const
	{
		return glm::vec2(cursorX, cursorY);
	}

	glm::vec2 IOSystem::getCursorOffset() const
	{
		glm::vec2 cursorOff;
		cursorOff.x = (float)cursorX - (float)lastX;
		cursorOff.y = (float)lastY - (float)cursorY;
		return cursorOff;
	}

	constexpr IOSystem::LibraryCode IOSystem::calculate(LibraryCode code, LibraryCode code1, LibraryCode code2, char op) const
	{
		switch (op)
		{
			case '+':
				return code + (code1 - code2);
				break;
			case '-':
				return code - (code1 - code2);
				break;
		}

		return -1;
	}

	constexpr IOSystem::LibraryCode IOSystem::mapLibraryToEngineKey(LibraryCode code) const
	{
		if (code == GLFW_KEY_SPACE) return KEY_SPACE;
		else if (code == GLFW_KEY_APOSTROPHE) return KEY_APOSTROPHE;
		else if (code < 3) return calculate(code, GLFW_MOUSE_BUTTON_MIDDLE, MOUSE_BUTTON_MIDDLE, '-');
		else if (code <= GLFW_KEY_Z) return calculate(code, GLFW_KEY_Z, KEY_Z, '-');
		else if (code <= GLFW_KEY_9) return calculate(code, GLFW_KEY_9, KEY_9, '-');
		else if (code <= GLFW_KEY_END) return calculate(code, GLFW_KEY_END, KEY_END, '-');
		else if (code <= GLFW_KEY_PAUSE) return calculate(code, GLFW_KEY_PAUSE, KEY_PAUSE, '-');
		else if (code <= GLFW_KEY_F12) return calculate(code, GLFW_KEY_F12, KEY_F12, '-');
		else if (code <= GLFW_KEY_KP_ENTER) return calculate(code, GLFW_KEY_KP_ENTER, KEY_KEYPAD_ENTER, '-');
		else if (code <= GLFW_KEY_RIGHT_SUPER) return calculate(code, GLFW_KEY_RIGHT_SUPER, KEY_RIGHT_SUPER, '-');

		return 100;
	}

	IOSystem::LibraryCode IOSystem::mapEngineToLibraryKey(KeyCode code) const
	{
		if (code == KEY_SPACE) return GLFW_KEY_SPACE;
		else if (code == KEY_APOSTROPHE) return GLFW_KEY_APOSTROPHE;
		else if (code < 3) return calculate(code, GLFW_MOUSE_BUTTON_MIDDLE, MOUSE_BUTTON_MIDDLE, '+');
		else if (code <= KEY_Z) return calculate(code, GLFW_KEY_Z, KEY_Z, '+');
		else if (code <= KEY_9) return calculate(code, GLFW_KEY_9, KEY_9, '+');
		else if (code <= KEY_END) return calculate(code, GLFW_KEY_END, KEY_END, '+');
		else if (code <= KEY_PAUSE) return calculate(code, GLFW_KEY_PAUSE, KEY_PAUSE, '+');
		else if (code <= KEY_F12) return calculate(code, GLFW_KEY_F12, KEY_F12, '+');
		else if (code <= KEY_KEYPAD_ENTER) return calculate(code, GLFW_KEY_KP_ENTER, KEY_KEYPAD_ENTER, '+');
		else if (code <= KEY_RIGHT_SUPER) return calculate(code, GLFW_KEY_RIGHT_SUPER, KEY_RIGHT_SUPER, '+');

		return 1000U;
	}
}