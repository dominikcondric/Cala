#pragma once
#include "Core/BaseApplication.h"
#include "GUI/UserInterface.h"
#include "Graphics/RenderingSystem.h"
#include "Core/Utility/FileDialog.h"

class Editor : public BaseApplication {
public:
	Editor(const Window::WindowSpecification& windowSpec, Cala::RenderingAPI api = Cala::RenderingAPI::OpenGL);
	~Editor() override = default;
	void run();
	void update() override;
	void onWindowResize(const glm::ivec2 windowSize) override;

private:
	void updateGuiWindows();
	void drawGUI();
	void arrangeGUI();
	void renderRendererStateOptions();
	void renderEntitiesList();
	void renderComponentList();
	void repositionElements();
	bool isCursorInsideViewport() const;
		
	PerspectiveCamera cam;
	UserInterface gui;
	ImGuiWindow leftPanel;
	ImGuiWindow topPanel;
	ImGuiWindow rightPanel;

	bool mouseHovered = false;
	bool guiUsed = false;
	glm::ivec4 sceneViewport{ 0, 0, 1440, 810 };

	// Scene
	Scene mainScene;
	Entity selectedEntity = -1;
};