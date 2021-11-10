#pragma once
#include "ImGuiWindow.h"
#include "Core/Window.h"
#include <list>

class UserInterface {
public:
	UserInterface(const Window& window);
	~UserInterface();
	void beginFrame() const;
	void endFrame() const;
	void addLabel(const char* label) const;
	
private:
	static bool imGuiInitialized;
};
