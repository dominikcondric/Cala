#pragma once
#include <imgui.h>
#include <glm/vec2.hpp>

class ImGuiWindow
{
public:
	ImGuiWindow(const char* _label);
	~ImGuiWindow() = default;
	void beginWindow();
	void endWindow();
	void setWindowViewport(const ImVec4& _viewport);
	void setWindowFlags(ImGuiWindowFlags _flags = 0);
	void setWindowMinSize(const ImVec2& minSize) { windowMinSize = minSize; }
	void setWindowMaxSize(const ImVec2& maxSize) { windowMaxSize = maxSize; }
	const ImVec4& getViewport() const { return viewport; }
	//void setColors(const ImVec4& border, const ImVec4& fill);
	float borderSize = 1.f;

private:
	const char* label;
	ImVec4 viewport;
	bool isOpen = false;
	ImGuiWindowFlags flags = 0;
	ImVec2 windowMinSize = ImVec2(0.f, 0.f);
	ImVec2 windowMaxSize = ImVec2(0.f, 0.f);
};

