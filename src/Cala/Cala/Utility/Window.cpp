#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Window.h"
#include "Platform.h"
#include "Logger.h"
#include "Cala/Rendering/GraphicsAPI.h"

#if defined(CALA_PLATFORM_WINDOWS)
	#define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(CALA_PLATFORM_LINUX)
	#define GLFW_EXPOSE_NATIVE_X11
#elif defined(CALA_PLATFORM_APPLE)
	#define GLFW_EXPOSE_NATIVE_COCOA
#endif

#include <GLFW/glfw3native.h>

namespace Cala {
	Window* Window::instance = nullptr;

	Window::Window(const Specification& specification) 
		: windowName(specification.windowName),
		  windowSize(specification.width, specification.height)
	{
		if (!glfwInit())
			Logger::getInstance().logErrorToConsole("Failed to initialize GLFW!");

		glfwWindowHint(GLFW_SAMPLES, specification.sampleCount);
		windowHandle = glfwCreateWindow(windowSize.x, windowSize.y, windowName.c_str(), NULL, NULL);
		if (!windowHandle)
		{
			glfwTerminate();
			Logger::getInstance().logErrorToConsole("Failed to create window!");
			return;
		}

		glfwSetWindowSizeCallback(windowHandle, windowResizeCallback);
		glfwSetInputMode(windowHandle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		glfwSetWindowUserPointer(windowHandle, this);
		ioSystem.reset(IOSystem::construct(windowHandle));

		glfwMakeContextCurrent(windowHandle);
		GraphicsAPI::loadAPIFunctions();
	}

	Window* Window::construct(const Specification& specification)
	{
		if (!instance)
			instance = new Window(specification);

		return instance;
	}

	Window::~Window()
	{
		instance = nullptr;
		glfwMakeContextCurrent(NULL);
		glfwTerminate();
	}

	void Window::update()
	{
		resized = false;
		ioSystem->update();
		glfwSwapBuffers(windowHandle);
	}

	bool Window::exitTriggered() const
	{
		return glfwWindowShouldClose(windowHandle);
	}

	const glm::ivec2& Window::getWindowSize() const
	{
		return windowSize;
	}

	void* Window::getNativeWindowPointer() const
	{
	#if defined GLFW_EXPOSE_NATIVE_WIN32
		return glfwGetWin32Window(windowHandle);
	#elif defined GLFW_EXPOSE_NATIVE_X11
		return (void*)glfwGetX11Window(windowHandle);
	#elif defined GLFW_EXPOSE_NATIVE_COCOA
		return glfwGetCocoaWindow(windowHandle);
	#endif
	}

	void Window::windowResizeCallback(GLFWwindow* window, int w, int h)
	{
		Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
		win->windowSize.x = w;
		win->windowSize.y = h;
		win->resized = true;
	}
}