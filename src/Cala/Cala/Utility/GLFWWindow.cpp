#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "GLFWWindow.h"
#include "Platform.h"
#include "Logger.h"
#include "Cala/Rendering/GraphicsAPI.h"
#include "GLFWIOSystem.h"

#if defined(CALA_PLATFORM_WINDOWS)
	#define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(CALA_PLATFORM_LINUX)
	#define GLFW_EXPOSE_NATIVE_X11
#elif defined(CALA_PLATFORM_APPLE)
	#define GLFW_EXPOSE_NATIVE_COCOA
#endif

#include <GLFW/glfw3native.h>

namespace Cala {
	GLFWWindow::GLFWWindow(const Specification& specification) : IWindow(specification) 
	{
		if (!glfwInit())
			Logger::getInstance().logErrorToConsole("Failed to initialize GLFW!");

		glfwWindowHint(GLFW_SAMPLES, sampleCount);
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
		ioSystem.reset(new GLFWIOSystem(windowHandle));

		glfwMakeContextCurrent(windowHandle);
		GraphicsAPI::loadAPIFunctions();
	}

	GLFWWindow::~GLFWWindow()
	{
		glfwMakeContextCurrent(NULL);
		glfwTerminate();
	}

	void GLFWWindow::update()
	{
		resized = false;
		ioSystem->update();
		glfwSwapBuffers(windowHandle);
	}

	bool GLFWWindow::exitTriggered() const
	{
		return glfwWindowShouldClose(windowHandle);
	}

	void* GLFWWindow::getNativeWindowPointer() const
	{
	#if defined GLFW_EXPOSE_NATIVE_WIN32
		return glfwGetWin32Window(windowHandle);
	#elif defined GLFW_EXPOSE_NATIVE_X11
		return (void*)glfwGetX11Window(windowHandle);
	#elif defined GLFW_EXPOSE_NATIVE_COCOA
		return glfwGetCocoaWindow(windowHandle);
	#endif
	}

	void GLFWWindow::windowResizeCallback(GLFWwindow* window, int w, int h)
	{
		GLFWWindow* win = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));
		win->windowSize.x = w;
		win->windowSize.y = h;
		win->resized = true;
	}
}