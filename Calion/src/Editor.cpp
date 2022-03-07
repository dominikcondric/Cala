#include "Editor.h"
#include "Path.h"
#include "imgui.h"

#define ROTATION_SPEED (2 * deltaTime)
#define TRANSLATION_SPEED (14 * deltaTime)

Editor::Editor(const Window::WindowSpecification& windowSpec, Cala::RenderingAPI api) : BaseApplication(windowSpec, api), gui(mainWindow),
leftPanel("Left panel"), topPanel("Menu bar"), rightPanel("RightPanel"), uiComponentProcessor(mainScene, -1, renderer.get())
{
	glm::ivec2 winSize = mainWindow.getWindowSize();
	cam.setProjectionAspectRatio((float)winSize.x / winSize.y);
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;
	leftPanel.setWindowFlags(flags);
	rightPanel.setWindowFlags(flags);
	flags |= ImGuiWindowFlags_NoResize;
	topPanel.setWindowFlags(flags);
	leftPanel.setWindowViewport(ImVec4(0.f, winSize.y / 20.f, winSize.x / 6.f, winSize.y - winSize.y / 20.f));
	rightPanel.setWindowViewport(ImVec4(winSize.x - winSize.x / 6.f, winSize.y / 20.f, winSize.x / 6.f, winSize.y - winSize.y / 20.f));
	topPanel.setWindowViewport(ImVec4(0, 0, (float)winSize.x, winSize.y / 20.f));
	leftPanel.setWindowMaxSize(ImVec2(winSize.x / 3.f, winSize.y - winSize.y / 20.f));
	leftPanel.setWindowMinSize(ImVec2(winSize.x / 7.f, winSize.y - winSize.y / 20.f));
	rightPanel.setWindowMaxSize(ImVec2(winSize.x / 3.f, winSize.y - winSize.y / 20.f));
	rightPanel.setWindowMinSize(ImVec2(winSize.x / 7.f, winSize.y - winSize.y / 20.f));
	repositionElements();

	std::filesystem::path texturesPath = Cala::getProjectPath() / "Calion/Textures/";

	cam.setPosition(glm::vec3(0.f, 5.f, 10.f));
	cam.setCenter({ 0.f, 0.f, 0.f });
}

void Editor::run()
{
	for (Entity drawable : mainScene.getComponentEntityList<RenderingComponent>())
		renderingSystem.pushToDrawQueue(drawable, mainScene.getComponent<RenderingComponent>(drawable));

	renderingSystem.draw(mainScene, cam);

	drawGUI();
}

void Editor::update()
{
	const IOSystem& io = mainWindow.getIO();
	float deltaTime = time.deltaTime;

	if (!ImGui::IsAnyItemActive())
	{
		if (io.isKeyPressed(IOSystem::KeyCode::KEY_W))
			cam.moveCamera(Camera::Directions::FORWARD, deltaTime);
		if (io.isKeyPressed(IOSystem::KeyCode::KEY_S))
			cam.moveCamera(Camera::Directions::BACKWARD, deltaTime);
		if (io.isKeyPressed(IOSystem::KeyCode::KEY_A))
			cam.moveCamera(Camera::Directions::LEFT, deltaTime);
		if (io.isKeyPressed(IOSystem::KeyCode::KEY_D))
			cam.moveCamera(Camera::Directions::RIGHT, deltaTime);
		if (io.isKeyPressed(IOSystem::KeyCode::KEY_SPACE))
			cam.moveCamera(Camera::Directions::UP, deltaTime);
		if (io.isKeyPressed(IOSystem::KeyCode::KEY_LEFT_SHIFT))
			cam.moveCamera(Camera::Directions::DOWN, deltaTime);
	}

	if (io.isMouseButtonPressed(IOSystem::KeyCode::MOUSE_BUTTON_RIGHT) && isCursorInsideViewport())
		cam.rotateCamera(io.getCursorOffset());

	guiUsed = false;
	mouseHovered = false;

	if (ImGui::IsAnyItemActive()) 
		guiUsed = true;

	if (ImGui::IsAnyItemHovered()) 
		mouseHovered = true;

	if (ImGui::GetIO().MouseClicked[0] && mouseHovered == false && selectedEntity != -1)
	{
		if (mainScene.hasComponent<RenderingComponent>(selectedEntity)) 
		{
			mainScene.getComponent<RenderingComponent>(selectedEntity).outlined = false;
		}

		selectedEntity = -1;
	}

	repositionElements();
	renderer->setViewport(sceneViewport);
}

bool Editor::isCursorInsideViewport() const
{
	glm::uvec2 cursorPosition = mainWindow.getIO().getCursorPosition();
	cursorPosition.y = mainWindow.getWindowSize().y - cursorPosition.y;
	return (cursorPosition.x > sceneViewport.x && cursorPosition.x < sceneViewport.x + sceneViewport.z && cursorPosition.y > sceneViewport.y && cursorPosition.y < sceneViewport.y + sceneViewport.w);
}

void Editor::onWindowResize(const glm::ivec2& windowSize)
{
	if (windowSize != glm::ivec2(0.f, 0.f))
	{
		repositionElements();

		renderer->setViewport(sceneViewport);
		cam.setProjectionAspectRatio(float(windowSize.x) / windowSize.y);

		leftPanel.setWindowMaxSize(ImVec2(windowSize.x / 3.f, windowSize.y - windowSize.y / 8.f));
		leftPanel.setWindowMinSize(ImVec2(windowSize.x / 7.f, windowSize.y - windowSize.y / 4.f));

		rightPanel.setWindowMaxSize(ImVec2(windowSize.x / 3.f, windowSize.y - windowSize.y / 8.f));
		rightPanel.setWindowMinSize(ImVec2(windowSize.x / 7.f, windowSize.y - windowSize.y / 4.f));

		topPanel.setWindowMaxSize(ImVec2((float)windowSize.x, windowSize.y / 4.f));
		topPanel.setWindowMinSize(ImVec2((float)windowSize.x, windowSize.y / 8.f));
	}
}

void Editor::drawGUI()
{
	gui.beginFrame();
	arrangeGUI();
	if (renderer->getSettingState(GraphicsAPI::GammaCorrection))
	{
		renderer->disableSetting(GraphicsAPI::GammaCorrection);
		gui.endFrame();
		renderer->enableSetting(GraphicsAPI::GammaCorrection);
	}
	else
	{
		gui.endFrame();
	} 
}

void Editor::arrangeGUI()
{
	leftPanel.beginWindow();
	renderEntitiesList();
	renderComponentList();
	leftPanel.endWindow();

	rightPanel.beginWindow();
	renderRendererStateOptions();
	rightPanel.endWindow();
}

void Editor::renderRendererStateOptions()
{
	ImGui::Text(("Frame rate: " + std::to_string((int)time.frameRate)).c_str());

	bool setting = renderer->getSettingState(GraphicsAPI::Multisampling);
	if (ImGui::Checkbox("Multisampling", &setting))
	{
		if (!setting)
			renderer->disableSetting(GraphicsAPI::Multisampling);
		else 
			renderer->enableSetting(GraphicsAPI::Multisampling);
	}

	setting = renderer->getSettingState(GraphicsAPI::Blending);
	if (ImGui::Checkbox("Blending", &setting))
	{
		if (!setting)
			renderer->disableSetting(GraphicsAPI::Blending);
		else
			renderer->enableSetting(GraphicsAPI::Blending);
	}

	setting = renderer->getSettingState(GraphicsAPI::GammaCorrection);
	if (ImGui::Checkbox("Gamma correction", &setting))
	{
		if (!setting)
			renderer->disableSetting(GraphicsAPI::GammaCorrection);
		else
			renderer->enableSetting(GraphicsAPI::GammaCorrection);
	}

	ImGui::Checkbox("Base grid", &renderingSystem.helperGridEnabled);
}

void Editor::renderEntitiesList()
{
	ImGui::BeginChild("##Entity list", ImVec2(0.f, ImGui::GetWindowHeight() / 2.f));
	if (ImGui::CollapsingHeader("Entities list"))
	{
		ImGui::PushTextWrapPos();
		ImGui::Indent();
		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.7f, 0.f, 0.4f, 1.f));

		for (Entity tagEntity : mainScene.getComponentEntityList<TagComponent>())
		{
			const TagComponent& tagComp = mainScene.getComponent<TagComponent>(tagEntity);
			bool selected = (selectedEntity == tagEntity);
			if (ImGui::Selectable(tagComp.name.c_str(), &selected, 0, ImVec2(ImGui::GetContentRegionAvailWidth(), 0.f)))
			{
				if (selectedEntity != -1 && selectedEntity != tagEntity && mainScene.hasComponent<RenderingComponent>(selectedEntity))
				{
					mainScene.getComponent<RenderingComponent>(selectedEntity).outlined = false;
				}

				selectedEntity = tagEntity;
				uiComponentProcessor.setEntity(selectedEntity);

				if (mainScene.hasComponent<RenderingComponent>(selectedEntity))
				{
					mainScene.getComponent<RenderingComponent>(selectedEntity).outlined = true;
				}
			}
		}

		if (selectedEntity != -1)
		{
			ImGui::Indent();

			uiComponentProcessor.positiveHasComponentCheck = true;
			processAllComponents(uiComponentProcessor);
			uiComponentProcessor.positiveHasComponentCheck = false;
			if (ImGui::BeginCombo("##Component list", uiComponentProcessor.getPreviewComponentName()))
			{
				processAllComponents(uiComponentProcessor);
				ImGui::EndCombo();
			}

			ImGui::SameLine();
			if (ImGui::Button("+", ImVec2(ImGui::GetContentRegionAvailWidth(), 0.f)))
			{
				uiComponentProcessor.addComponent();
			}

			if (ImGui::Button("Delete entity", ImVec2(ImGui::GetContentRegionAvailWidth(), 0.f)))
			{
				mainScene.removeEntity(selectedEntity);
				selectedEntity = -1;
			}
			ImGui::Unindent();
		}

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.f));
		if (ImGui::Button("New Entity", ImVec2(ImGui::GetContentRegionAvailWidth(), 0.f)))
		{
			selectedEntity = mainScene.addEntity();
			uiComponentProcessor.setEntity(selectedEntity);
		}
		ImGui::PopStyleColor();

		ImGui::PopStyleColor();
		ImGui::Unindent();
		ImGui::PopTextWrapPos();
	}
	ImGui::EndChild();
}

void Editor::renderComponentList()
{
	ImGui::PushTextWrapPos();
	ImGui::BeginChild("##Entity component list", ImVec2(0.f, ImGui::GetWindowHeight() / 2.f));
	if (selectedEntity != -1) 
	{
		uiComponentProcessor.createComponentUI(time.deltaTime);
	}
	ImGui::EndChild();
	ImGui::PopTextWrapPos();
}

void Editor::repositionElements()
{
	glm::vec2 winSize = mainWindow.getWindowSize();

	ImVec4 leftPanelViewport = leftPanel.getViewport();
	float topPanelHeight = topPanel.getViewport().w;
	leftPanel.setWindowViewport(ImVec4(leftPanelViewport.x, topPanelHeight, leftPanelViewport.z, winSize.y - topPanelHeight));

	ImVec4 rightPanelViewport = rightPanel.getViewport();
	rightPanel.setWindowViewport(ImVec4(winSize.x - rightPanelViewport.z, topPanelHeight, rightPanelViewport.z, winSize.y - topPanelHeight));

	sceneViewport.x = (int)leftPanel.getViewport().z;
	sceneViewport.y = 0;
	sceneViewport.z = (int)glm::ceil(winSize.x - rightPanel.getViewport().z - leftPanel.getViewport().z);
	sceneViewport.w = (int)glm::ceil(winSize.y - topPanel.getViewport().w);

	cam.setProjectionAspectRatio((float)sceneViewport.z / sceneViewport.w);
	renderer->setViewport(sceneViewport);
}