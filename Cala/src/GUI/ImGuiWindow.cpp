#include "ImGuiWindow.h"

ImGuiWindow::ImGuiWindow(const char* _label) : label(_label)
{
}

void ImGuiWindow::beginWindow()
{
	ImGui::SetNextWindowPos(ImVec2(viewport.x, viewport.y));
	ImGui::SetNextWindowSize(ImVec2(viewport.z, viewport.w));
	ImGui::SetNextWindowSizeConstraints(windowMinSize, windowMaxSize);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, borderSize);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, ImVec2(0.5f, 0.5f));
	ImGui::Begin(label, &isOpen, flags);
	ImGui::PopStyleVar(3);
	ImGui::PushTextWrapPos();
}

void ImGuiWindow::endWindow()
{
	const ImVec2& windowPos = ImGui::GetWindowPos();
	viewport.x = windowPos.x;
	viewport.y = windowPos.y;
	viewport.z = ImGui::GetWindowWidth();
	viewport.w = ImGui::GetWindowHeight();
	ImGui::PopTextWrapPos();
	ImGui::End();
}

void ImGuiWindow::setWindowFlags(ImGuiWindowFlags _flags)
{
	flags = _flags;
}

void ImGuiWindow::setWindowViewport(const ImVec4& _viewport)
{
	viewport = _viewport;
}