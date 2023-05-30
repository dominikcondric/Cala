#include "IIOSystem.h"
#include <GLFW/glfw3.h>
#include "GLFWIOSystem.h"

namespace Cala {
    GLFWIOSystem::GLFWIOSystem(GLFWwindow* _windowHandle) : windowHandle(_windowHandle)
    {
    }

    void GLFWIOSystem::update()
    {
        glfwPollEvents();
        IIOSystem::update();
    }

    constexpr GLFWIOSystem::LibraryCode GLFWIOSystem::calculate(LibraryCode code, LibraryCode code1, LibraryCode code2, char op) const
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

	constexpr GLFWIOSystem::LibraryCode GLFWIOSystem::mapLibraryToEngineKey(LibraryCode code) const
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

    glm::vec2 GLFWIOSystem::apiGetCursorPosition() const
    {
        double x, y;
        glfwGetCursorPos(windowHandle, &x, &y);
        return glm::vec2(x, y);
    }

    bool GLFWIOSystem::apiIsMouseButtonPressed(KeyCode code) const
    {
        return glfwGetMouseButton(windowHandle, mapEngineToLibraryKey(code));
    }

    bool GLFWIOSystem::apiIsKeyPressed(KeyCode code) const
    {
        return glfwGetKey(windowHandle, mapEngineToLibraryKey(code));
    }

    GLFWIOSystem::LibraryCode GLFWIOSystem::mapEngineToLibraryKey(KeyCode code) const
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