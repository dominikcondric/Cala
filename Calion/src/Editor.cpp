#include "Editor.h"
#include "Path.h"
#include "imgui.h"

#define ROTATION_SPEED (2 * deltaTime)
#define TRANSLATION_SPEED (14 * deltaTime)

Editor::Editor(const Window::WindowSpecification& windowSpec, GraphicsAPI::API api) : BaseApplication(windowSpec, api), gui(mainWindow),
leftPanel("Left panel"), topPanel("Menu bar"), rightPanel("RightPanel"), uiComponentProcessor(mainScene, -1, mainWindow, renderer.get()),
sceneLoadingDialog(mainWindow)
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

	cam.setPosition(glm::vec3(0.f, 5.f, 10.f));
	cam.setCenter({ 0.f, 0.f, 0.f });

	sceneLoadingDialog.setInitialDirectory((Cala::getProjectPath() / "Calion/Scenes/").string());
	sceneLoadingDialog.setFilters("Cala scene (.csn)\0*.csn\0\0");
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
	renderPostProcessingEffects();
	rightPanel.endWindow();
}

void Editor::renderPostProcessingEffects()
{
	ImGui::BeginChild("##PostProcessingEffects", ImVec2(0.f, ImGui::GetWindowHeight() / 2.f));
	ImGui::Text("Post processing effects:");
	ImGui::Indent();

	bool effectState = renderingSystem.getPostProcessingEffectState(RenderingSystem::Negative);
	if (ImGui::Checkbox("Negative", &effectState))
		renderingSystem.setPostProcessingEffectState(RenderingSystem::Negative, effectState);

	effectState = renderingSystem.getPostProcessingEffectState(RenderingSystem::BoxBlur);
	if (ImGui::Checkbox("Box blur", &effectState))
		renderingSystem.setPostProcessingEffectState(RenderingSystem::BoxBlur, effectState);

	ImGui::Indent();
	if (effectState)
	{
		ImGui::SliderFloat("##BoxBlurLevel", &renderingSystem.getPostProcessingEffectValue(RenderingSystem::BoxBlur), 0.f, 800.f);
	}
	ImGui::Unindent();

	effectState = renderingSystem.getPostProcessingEffectState(RenderingSystem::GaussianBlur);
	if (ImGui::Checkbox("Gaussian blur", &effectState))
		renderingSystem.setPostProcessingEffectState(RenderingSystem::GaussianBlur, effectState);

	ImGui::Indent();
	if (effectState)
	{
		ImGui::SliderFloat("##BoxBlurLevel", &renderingSystem.getPostProcessingEffectValue(RenderingSystem::GaussianBlur), 0.f, 800.f);
	}
	ImGui::Unindent();

	effectState = renderingSystem.getPostProcessingEffectState(RenderingSystem::EdgeDetection);
	if (ImGui::Checkbox("Edge detection", &effectState))
		renderingSystem.setPostProcessingEffectState(RenderingSystem::EdgeDetection, effectState);

	ImGui::Indent();
	if (effectState)
	{
		ImGui::SliderFloat("##BoxBlurLevel", &renderingSystem.getPostProcessingEffectValue(RenderingSystem::EdgeDetection), 0.f, 800.f);
	}
	ImGui::Unindent();

	effectState = renderingSystem.getPostProcessingEffectState(RenderingSystem::HDR);
	if (ImGui::Checkbox("HDR", &effectState))
		renderingSystem.setPostProcessingEffectState(RenderingSystem::HDR, effectState);

	ImGui::Indent();
	if (effectState)
	{
		ImGui::SliderFloat("##BoxBlurLevel", &renderingSystem.getPostProcessingEffectValue(RenderingSystem::HDR), 0.f, 10.f);
	}
	ImGui::Unindent();

	effectState = renderingSystem.getPostProcessingEffectState(RenderingSystem::Bloom);
	if (ImGui::Checkbox("Bloom", &effectState))
		renderingSystem.setPostProcessingEffectState(RenderingSystem::Bloom, effectState);

	ImGui::Indent();
	if (effectState)
	{
		ImGui::SliderFloat("##Bloom level", &renderingSystem.getPostProcessingEffectValue(RenderingSystem::Bloom), 0.f, 10.f);
	}
	ImGui::Unindent();

	ImGui::Unindent();
	ImGui::EndChild();
}

void Editor::renderRendererStateOptions()
{
	ImGui::BeginChild("##RendererOptions", ImVec2(0.f, ImGui::GetWindowHeight() / 2.f));
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
	ImGui::EndChild();
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
			if (ImGui::BeginCombo("##AddComponentList", uiComponentProcessor.getPreviewComponentName()))
			{
				processAllComponents(uiComponentProcessor);
				ImGui::EndCombo();
			}

			ImGui::SameLine();
			if (ImGui::Button("+", ImVec2(ImGui::GetContentRegionAvailWidth(), 0.f)))
			{
				uiComponentProcessor.addSelectedComponent();
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

	ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 9.f);
	ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));
	if (ImGui::Button("Load\nscene", ImVec2(ImGui::GetWindowWidth() / 3.f, 0.f)))
	{
		std::string sceneFile = sceneLoadingDialog.openFile();
		sceneSerializer.loadScene(&mainScene, sceneFile);
	}

	ImGui::SameLine();
	if (ImGui::Button("Save\nscene", ImVec2(ImGui::GetWindowWidth() / 3.f, 0.f)))
	{
		std::string saveFileName = sceneLoadingDialog.saveFile();
		if (!saveFileName.empty())
		{
			sceneSerializer.serializeScene(&mainScene, saveFileName);
		}
	}
	ImGui::PopStyleVar();

	ImGui::EndChild();
}

void Editor::renderComponentList()
{
	ImGui::PushTextWrapPos();
	ImGui::BeginChild("##Entity component list", ImVec2(0.f, ImGui::GetWindowHeight() / 2.f));
	if (selectedEntity != -1) 
	{
		uiComponentProcessor.createSelectedComponentUI(time.deltaTime);
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