#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Window.h"
#include "Core.h"

#if defined(CALA_PLATFORM_WINDOWS)
	#define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(CALA_PLATFORM_LINUX)
	#define GLFW_EXPOSE_NATIVE_X11
#elif defined(CALA_PLATFORM_APPLE)
	#define GLFW_EXPOSE_NATIVE_COCOA
#endif

#include <GLFW/glfw3native.h>

namespace Cala {
	Window::Window(const WindowSpecification& specification)
	{
		if (!glfwInit())
			LOG_ERROR("Failed to initialize GLFW!");

		windowName = specification.windowName;
		glfwWindowHint(GLFW_SAMPLES, specification.sampleCount);
		windowHandle = glfwCreateWindow(specification.width, specification.height, windowName.c_str(), NULL, NULL);
		if (!windowHandle)
		{
			shutdown();
			LOG_ERROR("Failed to create window!");
		}

		glfwSetWindowSizeCallback(windowHandle, windowResizeCallback);
		glfwSetInputMode(windowHandle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		glfwSetWindowUserPointer(windowHandle, this);
		ioSystem = std::make_unique<IOSystem>(windowHandle);
	}

	Window::~Window()
	{
		shutdown();
	}

	void Window::shutdown()
	{
		glfwTerminate();
	}

	void Window::createOpenGLContext() const
	{
		glfwMakeContextCurrent(windowHandle);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			LOG_ERROR("Failed to initialize GLAD");
		}
	}

	void Window::update()
	{
		resized = false;
		ioSystem->update();
		glfwSwapBuffers(windowHandle);
		time.update();
	}

	bool Window::exitTriggered() const
	{
		return glfwWindowShouldClose(windowHandle);
	}

	glm::ivec2 Window::getWindowSize() const
	{
		int w, h;
		glfwGetWindowSize(windowHandle, &w, &h);
		return glm::ivec2(w, h);
	}

	void* const Window::getNativeWindowPointer() const
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
		win->resized = true;
	}
}