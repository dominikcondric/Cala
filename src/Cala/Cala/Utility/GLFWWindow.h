#pragma once
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <memory>
#include <string>
#include "IIOSystem.h"
#include "Time.h"
#include "IWindow.h"

struct GLFWwindow;

namespace Cala {
	class GLFWWindow : public IWindow {
	public:
		GLFWWindow(const Specification& specification);
		~GLFWWindow() override;
		void update() override;
		bool exitTriggered() const override;
		bool isResized() const override { return resized; }
		GLFWwindow* getWindowPointer() { return windowHandle; }
		void* getNativeWindowPointer() const override;

	private:
		static void windowResizeCallback(GLFWwindow* window, int w, int h);

	private:
		GLFWwindow* windowHandle = nullptr;
		bool resized = false;
	};
}