#pragma once
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <memory>
#include <string>
#include "IOSystem.h"
#include "Core.h"
#include "Time.h"

struct GLFWwindow;

namespace Cala {
	class Window {
	public:
		struct Specification {
			Specification(const std::string& _windowName, uint32_t _width, uint32_t _height, uint32_t _sampleCount) :
				windowName(_windowName), width(_width), height(_height), sampleCount(_sampleCount) {}
			std::string windowName;
			uint32_t width;
			uint32_t height;
			uint32_t sampleCount;
		};

	public:
		static Window* construct(const Specification& specification);
		~Window();
		void update();
		bool exitTriggered() const;
		bool isResized() const { return resized; }
		const IOSystem& getIO() const { return *ioSystem.get(); }
		const Time& getTime() const { return time; }
		glm::ivec2 getWindowSize() const;
		GLFWwindow* const getWindowPointer() const { return windowHandle; }
		void* const getNativeWindowPointer() const;

	private:
		Window(const Specification& specification);
		static void windowResizeCallback(GLFWwindow* window, int w, int h);
		void createContext() const;

		Time time;
		Unique<IOSystem> ioSystem;
		GLFWwindow* windowHandle = nullptr;
		std::string windowName;
		bool resized = false;
		static bool initialized;
	};
}