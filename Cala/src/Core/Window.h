#pragma once
#include "Core/Core.h"
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include "Core/IOSystem.h"
#include <memory>

struct GLFWwindow;

class Window {
public:
	struct WindowSpecification {
		std::string windowName;
		int width;
		int height;
		uint32_t sampleCount;
	};

public:
	Window(const WindowSpecification& specification);
	~Window();
	void shutdown();
	void createOpenGLContext() const;
	//void createDirectXContext() const;
	//void createVulkanContext() const;
	void update();
	void swapBuffers() const;
	bool exitTriggered() const;
	bool isResized() const { return resized; }
	const IOSystem& getIO() const { return *ioSystem; }
	glm::ivec2 getWindowSize() const;
	GLFWwindow* const getWindowPointer() const { return windowHandle; }
	void* const getNativeWindowPointer() const;

private:
	Unique<IOSystem> ioSystem;
	GLFWwindow* windowHandle = nullptr;
	std::string windowName;
	bool resized = false;
	static void windowResizeCallback(GLFWwindow* window, int w, int h);
};