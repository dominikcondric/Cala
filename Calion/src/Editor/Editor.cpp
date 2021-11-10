#include "Editor.h"
#include "Path.h"
#include "imgui.h"

#define ROTATION_SPEED (2 * deltaTime)
#define TRANSLATION_SPEED (14 * deltaTime)

Editor::Editor(const Window::WindowSpecification& windowSpec, Cala::RenderingAPI api) : BaseApplication(windowSpec, api), gui(mainWindow),
	leftPanel("Left panel"), topPanel("Tools"), rightPanel("RightPanel")
{
	glm::ivec2 winSize = mainWindow.getWindowSize();
	cam.setProjectionAspectRatio((float)winSize.x / winSize.y);
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;
	leftPanel.setWindowFlags(flags);
	rightPanel.setWindowFlags(flags);
	flags |= ImGuiWindowFlags_NoTitleBar;
	topPanel.setWindowFlags(flags);
	leftPanel.setWindowViewport(ImVec4(0.f, winSize.y / 5.f, winSize.x / 6.f, winSize.y - winSize.y / 5.f));
	rightPanel.setWindowViewport(ImVec4(winSize.x - winSize.x / 6.f, winSize.y / 5.f, winSize.x / 6.f, winSize.y - winSize.y / 5.f));
	topPanel.setWindowViewport(ImVec4(0, 0, (float)winSize.x, winSize.y / 5.f));
	leftPanel.setWindowMaxSize(ImVec2(winSize.x / 3.f, winSize.y - winSize.y / 8.f));
	leftPanel.setWindowMinSize(ImVec2(winSize.x / 7.f, winSize.y - winSize.y / 4.f));
	rightPanel.setWindowMaxSize(ImVec2(winSize.x / 3.f, winSize.y - winSize.y / 8.f));
	rightPanel.setWindowMinSize(ImVec2(winSize.x / 7.f, winSize.y - winSize.y / 4.f));
	topPanel.setWindowMaxSize(ImVec2((float)winSize.x, winSize.y / 4.f));
	topPanel.setWindowMinSize(ImVec2((float)winSize.x, winSize.y / 8.f));
	repositionElements();

	std::filesystem::path texturesPath = Cala::getProjectPath() / "Calion/Textures/";
	
	// Sphere
	Entity sphere = mainScene.addEntity("Sphere model");
	mainScene.addComponent<RenderingComponent>(sphere);
	RenderingComponent& sphereRenderingComp = mainScene.getComponent<RenderingComponent>(sphere);
	sphereRenderingComp.mesh.reset(renderer->createMesh());
	sphereRenderingComp.mesh->loadSphere();
	mainScene.getComponent<TransformComponent>(sphere).scale(glm::vec3(2.f));

	// Box
	Entity box = mainScene.addEntity("Wooden box");
	mainScene.addComponent<RenderingComponent>(box);
	mainScene.addComponent<TextureComponent>(box);
	RenderingComponent& boxRenderingComp = mainScene.getComponent<RenderingComponent>(box);
	boxRenderingComp.mesh.reset(renderer->createMesh());
	boxRenderingComp.mesh->loadCube();
	boxRenderingComp.lightened = true;

	TextureComponent& boxTextureComp = mainScene.getComponent<TextureComponent>(box);
	boxTextureComp.diffuseMap.reset(renderer->createTexture());
	boxTextureComp.specularMap.reset(renderer->createTexture());
	Texture::TextureSpecification textureSpec;
	textureSpec.gammaCorrection = renderer->getSettingState(GraphicsAPI::GammaCorrection);
	boxTextureComp.diffuseMap->load2DTexture(Image(texturesPath / "containerDiffuse.png"), textureSpec);
	boxTextureComp.specularMap->load2DTexture(Image(texturesPath / "containerSpecular.png"), textureSpec);

	mainScene.getComponent<TransformComponent>(box).translate(glm::vec3(3.f, 0.f, 0.f));
	
	// Light 1
	Entity light1 = mainScene.addEntity("Light1");
	mainScene.addComponent<RenderingComponent>(light1);
	mainScene.addComponent<LightComponent>(light1);
	RenderingComponent& firstLightRenderingComp = mainScene.getComponent<RenderingComponent>(light1);
	firstLightRenderingComp.mesh.reset(renderer->createMesh());
	firstLightRenderingComp.mesh->loadSphere();
	firstLightRenderingComp.color = glm::vec4(1.f, 0.1f, 0.0f, 1.f);
	firstLightRenderingComp.lightened = false;

	TransformComponent& firstLightTransformComp = mainScene.getComponent<TransformComponent>(light1);
	firstLightTransformComp.translate(glm::vec3(2.f, 4.f, -2.f));
	firstLightTransformComp.scale(glm::vec3(0.2f));

	LightComponent& firstLightComp = mainScene.getComponent<LightComponent>(light1);
	firstLightComp.color = glm::vec3(1.f, 0.f, 0.f);
	firstLightComp.type = LightComponent::LightSourceType::Point;

	// Light 2
	Entity light2 = mainScene.addEntity("Light2");
	mainScene.addComponent<RenderingComponent>(light2);
	mainScene.addComponent<LightComponent>(light2);
	RenderingComponent& secondLightRenderingComp = mainScene.getComponent<RenderingComponent>(light2);
	secondLightRenderingComp.mesh.reset(renderer->createMesh());
	secondLightRenderingComp.mesh = firstLightRenderingComp.mesh;
	secondLightRenderingComp.color = glm::vec4(1.f);
	secondLightRenderingComp.lightened = false;
	TransformComponent& secondLightTransformComp = mainScene.getComponent<TransformComponent>(light2);
	secondLightTransformComp.translate(glm::vec3(-2.f, 0.f, -2.f));
	secondLightTransformComp.scale(glm::vec3(0.2f));

	LightComponent& secondLightComponent = mainScene.getComponent<LightComponent>(light2);
	secondLightComponent.color = glm::vec3(1.f);
	secondLightComponent.type = LightComponent::LightSourceType::Point;

	// Skybox
	Entity skybox = mainScene.addEntity("skybox");
	mainScene.addComponent<SkyboxComponent>(skybox);
	SkyboxComponent& skyboxComp = mainScene.getComponent<SkyboxComponent>(skybox);

	std::array<Image, 6> skyboxImages;
	skyboxImages[0].loadImage(texturesPath / "cubemap0.jpg");
	skyboxImages[1].loadImage(texturesPath / "cubemap1.jpg");
	skyboxImages[2].loadImage(texturesPath / "cubemap2.jpg");
	skyboxImages[3].loadImage(texturesPath / "cubemap3.jpg");
	skyboxImages[4].loadImage(texturesPath / "cubemap4.jpg");
	skyboxImages[5].loadImage(texturesPath / "cubemap5.jpg");

	skyboxComp.texture.reset(renderer->createTexture());
	skyboxComp.texture->loadCubemap(skyboxImages, textureSpec);
	skyboxComp.blurLevel = 200;

	cam.setPosition(glm::vec3(0.f, 5.f, 10.f));
	cam.setCenter({ 0.f, 0.f, 0.f });

	renderingSystem.helperGridEnabled = true;
}

void Editor::run()
{
	//for (const auto& entity : mainScene.getComponentCollection<ScriptingComponent>())
		//entity.component.getScript()->onUpdate(mainScene, entity.entity, time, mainWindow.getIO());

	renderingSystem.setSkybox(mainScene.getComponentEntityList<SkyboxComponent>()[0]);
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

	updateGuiWindows();

	
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

void Editor::onWindowResize(const glm::ivec2 windowSize)
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

void Editor::updateGuiWindows()
{
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
	leftPanel.endWindow();

	rightPanel.beginWindow();
	renderRendererStateOptions();
	rightPanel.endWindow();

	topPanel.beginWindow();
	topPanel.endWindow();
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
}

void Editor::renderEntitiesList()
{
	if (ImGui::CollapsingHeader("Entities list"))
	{
		ImGui::PushTextWrapPos();
		ImGui::Indent();
		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.7f, 0.f, 0.4f, 1.f));

		for (Entity tagEntity : mainScene.getComponentEntityList<TagComponent>())
		{
			const TagComponent& tag = mainScene.getComponent<TagComponent>(tagEntity);
			bool selected = (selectedEntity == tagEntity);
			if (ImGui::Selectable(tag.name.c_str(), &selected, 0, ImVec2(ImGui::GetContentRegionAvailWidth(), 0.f)))
			{
				if (selectedEntity != -1 && selectedEntity != tagEntity && mainScene.hasComponent<RenderingComponent>(selectedEntity))
					mainScene.getComponent<RenderingComponent>(selectedEntity).outlined = false;

				selectedEntity = tagEntity;
				if (mainScene.hasComponent<RenderingComponent>(selectedEntity))
				{
					mainScene.getComponent<RenderingComponent>(selectedEntity).outlined = true;
				}
			}
		}

		if (selectedEntity != -1)
		{
			if (ImGui::Button("Delete entity", ImVec2(ImGui::GetContentRegionAvailWidth(), 40.f)))
			{
				
				mainScene.removeEntity(selectedEntity);
				selectedEntity = -1;
			}
		}

		ImGui::PopStyleColor();
		ImGui::Unindent();
		ImGui::PopTextWrapPos();
	}
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