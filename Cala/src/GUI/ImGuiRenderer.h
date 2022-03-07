#pragma once
#include "ImGuiWindow.h"
#include "Core/Window.h"

class ImGuiRenderer {
public:
	ImGuiRenderer(const Window& window);
	~ImGuiRenderer();
	void beginFrame() const;
	void endFrame() const;
	
private:
	static bool imGuiInitialized;
};
