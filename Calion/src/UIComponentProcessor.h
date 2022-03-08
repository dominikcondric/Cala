#pragma once
#include "Core/Core.h"
#include "ECS/Components.h"
#include "imgui.h"
#include "ECS/ComponentIterator.h"
#include "ECS/Scene.h"
#include "Graphics/GraphicsAPI.h"
#include "Core/Utility/FileDialog.h"

class UIComponentProcessor : public ComponentProcessor {
public:
	UIComponentProcessor(Scene& scene, Entity entityID, const Window& window, const GraphicsAPI* api);
	~UIComponentProcessor() = default;
	void setEntity(Entity entityID);
	const char* getPreviewComponentName() const { return previewComponent; }
	void createSelectedComponentUI(float deltaTime);
	void addSelectedComponent();

	template<typename Type> void process();
	template<typename Type> constexpr const char* getComponentName() const { return "Unset"; }
	template<typename Type> void createRemoveComponentButton();

	bool positiveHasComponentCheck = true;

private:
	Scene& scene;
	Entity entity;
	const GraphicsAPI* api;
	const char* previewComponent = nullptr;
	const char* selectedComponent = nullptr;

	// Rendering component editor
	const char* models[3] = { "Cube", "Sphere", "Plane" };
	const char* previewModel = models[0];

	// Tag component editor
	char tagNameBuffer[50] = "";

	// Texture component editor
	FileDialog texturesFileDialog;
	
	template<typename Type> void createComponentUI(float);
	template<typename Type> void addComponent();

	using ComponentUIFunction = void (UIComponentProcessor::*)(float);
	using ComponentAddFunction = void (UIComponentProcessor::*)();
	ComponentAddFunction compAddFunction = nullptr;
	ComponentUIFunction compModifyFunction = nullptr;
};

template<> inline constexpr const char* UIComponentProcessor::getComponentName<RenderingComponent>() const { return "Rendering component"; }
template<> inline constexpr const char* UIComponentProcessor::getComponentName<LightComponent>() const { return "Light component"; }
template<> inline constexpr const char* UIComponentProcessor::getComponentName<TextureComponent>() const { return "Texture component"; }
template<> inline constexpr const char* UIComponentProcessor::getComponentName<ScriptingComponent>() const { return "Scripting component"; }
template<> inline constexpr const char* UIComponentProcessor::getComponentName<TagComponent>() const { return "Tag component"; }
template<> inline constexpr const char* UIComponentProcessor::getComponentName<TransformComponent>() const { return "Transform component"; }
template<> inline constexpr const char* UIComponentProcessor::getComponentName<SkyboxComponent>() const { return "Skybox component"; }

template<typename Type>
inline void UIComponentProcessor::process()
{
	const char* componentName = getComponentName<Type>();
	if (!scene.hasComponent<Type>(entity) && !positiveHasComponentCheck)
	{
		bool isSelected = (previewComponent == componentName);
		if (previewComponent == nullptr || ImGui::Selectable(componentName, isSelected))
		{
			compAddFunction = &UIComponentProcessor::addComponent<Type>;
			previewComponent = componentName;

			if (isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
	}
	else if (scene.hasComponent<Type>(entity) && positiveHasComponentCheck)
	{
		bool isSelected = (selectedComponent == componentName);
		if (selectedComponent == nullptr || ImGui::Selectable(componentName, isSelected))
		{
			compModifyFunction = &UIComponentProcessor::createComponentUI<Type>;
			selectedComponent = componentName;
			if (isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
	}
}

template<typename Type>
inline void UIComponentProcessor::createRemoveComponentButton()
{
	if (ImGui::Button("Remove component", ImVec2(ImGui::GetContentRegionAvailWidth(), 0.f)))
	{
		scene.template removeComponent<Type>(entity);
		compModifyFunction = nullptr;
	}
}

template<typename Type> 
inline void UIComponentProcessor::createComponentUI(float deltaTime)
{
	LOG("Unimplemented");
}

template<typename Type>
inline void UIComponentProcessor::addComponent()
{
	scene.addComponent<Type>(entity);
	previewComponent = nullptr;
	compAddFunction = nullptr;
	compModifyFunction = &UIComponentProcessor::createComponentUI<Type>;
}

template<>
inline void UIComponentProcessor::createComponentUI<RenderingComponent>(float)
{
	RenderingComponent& component = scene.getComponent<RenderingComponent>(entity);
	ImGui::Checkbox("Lightened", &component.lightened);
	ImGui::Checkbox("Culled", &component.culled);
	ImGui::Checkbox("Outlined", &component.outlined);
	float* pickedColor = &component.color.x;
	ImGui::ColorPicker4("##", pickedColor, ImGuiColorEditFlags_NoLabel);
	if (ImGui::BeginCombo("Model", previewModel))
	{
		for (uint32_t i = 0; i < sizeof(models) / sizeof(char*); ++i)
		{
			bool isSelected = (previewModel == models[i]);
			if (ImGui::Selectable(models[i], isSelected))
			{
				previewModel = models[i];
				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}

		}
		ImGui::EndCombo();
	}

	ImGui::SameLine();
	if (ImGui::Button("Add model"))
	{
		Mesh* mesh = GraphicsAPI::createMesh();
		if (strcmp(previewModel, "Cube") == 0)
			mesh->loadCube();
		else if (strcmp(previewModel, "Sphere") == 0)
			mesh->loadSphere();
		else
			mesh->loadPlane();

		component.mesh.reset(mesh);
	}
	createRemoveComponentButton<RenderingComponent>();
}

template<>
inline void UIComponentProcessor::createComponentUI<TagComponent>(float)
{
	TagComponent& tagComp = scene.getComponent<TagComponent>(entity);
	if (strlen(tagNameBuffer) == 0)
		memcpy(tagNameBuffer, tagComp.name.c_str(), sizeof(tagNameBuffer));

	ImGui::InputText("Tag name", tagNameBuffer, sizeof(tagNameBuffer));
	ImGui::SetCursorPosX(ImGui::GetContentRegionAvailWidth() / 4.f);
	if (ImGui::Button("Save", ImVec2(ImGui::GetContentRegionAvailWidth() / 2.f, 0.f)))
	{
		tagComp.name.assign(tagNameBuffer);
		tagNameBuffer[0] = '\0';
	}
}

template<>
inline void UIComponentProcessor::createComponentUI<LightComponent>(float deltaTime)
{
	LightComponent& component = scene.getComponent<LightComponent>(entity);
	ImGui::Text("Light type:");
	ImGui::Indent();
	if (ImGui::RadioButton("Point", component.type == LightComponent::LightSourceType::Point))
		component.type = LightComponent::LightSourceType::Point;
	if (ImGui::RadioButton("Cone", component.type == LightComponent::LightSourceType::Cone))
		component.type = LightComponent::LightSourceType::Cone;
	if (ImGui::RadioButton("Directional", component.type == LightComponent::LightSourceType::Directional))
		component.type = LightComponent::LightSourceType::Directional;
	ImGui::Unindent();

	ImGui::Text("Light intensity:");
	ImGui::Indent();
	ImGui::SliderFloat("##LightIntensity", &component.intensity, 0.f, 5.f);
	ImGui::Unindent();

	
	if (component.type == LightComponent::LightSourceType::Cone)
	{
		ImGui::Text("Cone cutoff:");
		ImGui::Indent();
		ImGui::SliderAngle("Cone cutoff", &component.coneCutoff, 0.f, 160.f);
		ImGui::Unindent();
	}

	ImGui::Text("Color:");
	ImGui::Indent();
	ImGui::ColorEdit3("##LightColor", &component.color.x);
	ImGui::Unindent();
}

template<>
inline void UIComponentProcessor::createComponentUI<TextureComponent>(float deltaTime)
{
	TextureComponent& component = scene.getComponent<TextureComponent>(entity);
	Texture::TextureSpecification texSpec;
	texSpec.gammaCorrection = api->getSettingState(GraphicsAPI::GammaCorrection);

	ImGui::PushTextWrapPos();
	ImGui::Text("Diffuse map:");
	ImGui::Indent();
	if (component.diffuseMap != nullptr)
	{
		ImGui::Text(component.diffuseMap->getImagePath().c_str());
		if (ImGui::Button("Remove##diffuse map", ImVec2(ImGui::GetContentRegionAvailWidth() / 2.f, 0.f)))
			component.diffuseMap = nullptr;
	}
	else
	{
		if (ImGui::Button("Add##difuse map", ImVec2(ImGui::GetContentRegionAvailWidth() / 2.f, 0.f)))
		{
			std::string imagePath = texturesFileDialog.openFile();
			if (!imagePath.empty())
			{
				component.diffuseMap.reset(GraphicsAPI::createTexture()); 
				component.diffuseMap->load2DTexture(Image(imagePath), texSpec);
			}
		}
	}
	ImGui::Unindent();

	ImGui::Text("Specular map:");
	ImGui::Indent();
	if (component.specularMap != nullptr)
	{
		ImGui::Text(component.specularMap->getImagePath().c_str());
		if (ImGui::Button("Remove##specular map", ImVec2(ImGui::GetContentRegionAvailWidth() / 2.f, 0.f)))
			component.specularMap = nullptr;
	}
	else
	{
		if (ImGui::Button("Add##specular map", ImVec2(ImGui::GetContentRegionAvailWidth() / 2.f, 0.f)))
		{
			std::string imagePath = texturesFileDialog.openFile();
			if (!imagePath.empty())
			{
				component.specularMap.reset(GraphicsAPI::createTexture());
				component.specularMap->load2DTexture(Image(imagePath), texSpec);
			}
		}
	}
	ImGui::Unindent();

	ImGui::Text("Normal map:");
	ImGui::Indent();
	if (component.normalMap != nullptr)
	{
		ImGui::Text(component.normalMap->getImagePath().c_str());
		if (ImGui::Button("Remove##normal map", ImVec2(ImGui::GetContentRegionAvailWidth() / 2.f, 0.f)))
			component.normalMap = nullptr;
	}
	else
	{
		if (ImGui::Button("Add##normal map", ImVec2(ImGui::GetContentRegionAvailWidth() / 2.f, 0.f)))
		{
			std::string imagePath = texturesFileDialog.openFile();
			if (!imagePath.empty())
			{
				component.normalMap.reset(GraphicsAPI::createTexture());
				component.normalMap->load2DTexture(Image(imagePath), texSpec);
			}
		}
	}
	ImGui::Unindent();
	ImGui::PopTextWrapPos();
}


template<>
inline void UIComponentProcessor::createComponentUI<TransformComponent>(float deltaTime)
{
	TransformComponent& component = scene.getComponent<TransformComponent>(entity);
	glm::vec3 translation(0.f);
	bool changed = false;
	ImGui::Text("Translation:");
	ImGui::Indent();
	changed |= ImGui::SliderFloat("X-translation", &translation.x, -5.f, 5.f);
	changed |= ImGui::SliderFloat("Y-translation", &translation.y, -5.f, 5.f);
	changed |= ImGui::SliderFloat("Z-translation", &translation.z, -5.f, 5.f);
	ImGui::Unindent();
	if (changed)
	{
		component.translate(translation * deltaTime);
	}

	changed = false;
	glm::vec3 scale(1.f);
	ImGui::Text("Scale:");
	ImGui::Indent();
	changed |= ImGui::SliderFloat("X-scale", &scale.x, 0.f, 2.f);
	changed |= ImGui::SliderFloat("Y-scale", &scale.y, 0.f, 2.f);
	changed |= ImGui::SliderFloat("Z-scale", &scale.z, 0.f, 2.f);
	float uniformScale = 1.f;
	if (ImGui::SliderFloat("Uniform", &uniformScale, 0.f, 2.f))
	{
		scale.y = scale.z = scale.x = uniformScale;
		changed = true;
	}
	ImGui::Unindent();
	if (changed)
	{
		component.scale((scale - 1.f) * deltaTime + 1.f);
	}

	changed = false;
	glm::vec3 rotation(0.f);
	ImGui::Text("Rotation:");
	ImGui::Indent();
	changed |= ImGui::SliderFloat("X-rotation", &rotation.x, -10.f, 10.f);
	changed |= ImGui::SliderFloat("Y-rotation", &rotation.y, -10.f, 10.f);
	changed |= ImGui::SliderFloat("Z-rotation", &rotation.z, -10.f, 10.f);
	ImGui::Unindent();
	if (changed)
	{
		component.rotate(rotation.x * deltaTime, glm::vec3(1.f, 0.f, 0.f));
		component.rotate(rotation.y * deltaTime, glm::vec3(0.f, 1.f, 0.f));
		component.rotate(rotation.z * deltaTime, glm::vec3(0.f, 0.f, 1.f));
	}
}
