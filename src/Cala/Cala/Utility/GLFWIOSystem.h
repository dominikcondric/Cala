#pragma once
#include "IIOSystem.h"

struct GLFWwindow;

namespace Cala {
	class GLFWIOSystem : public IIOSystem {
	public:
		GLFWIOSystem(GLFWwindow* _windowHandle);
		~GLFWIOSystem() = default;
        void update() override;

	protected:
		glm::vec2 apiGetCursorPosition() const override;
		bool apiIsMouseButtonPressed(KeyCode code) const override;
		bool apiIsKeyPressed(KeyCode code) const override;

	private:
		using LibraryCode = int;
		GLFWwindow* windowHandle = nullptr;
		LibraryCode mapEngineToLibraryKey(KeyCode code) const;
		constexpr LibraryCode mapLibraryToEngineKey(LibraryCode code) const;
		constexpr LibraryCode calculate(LibraryCode code, LibraryCode code1, LibraryCode code2, char op) const;
	};
}