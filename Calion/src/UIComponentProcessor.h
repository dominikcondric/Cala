#pragma once
#include "Core/Core.h"
#include "ECS/Components.h"
#include "imgui.h"
#include "ECS/ComponentIterator.h"
#include "ECS/Scene.h"

class UIComponentProcessor : public ComponentProcessor {
public:
	UIComponentProcessor(Scene& scene, Entity entityID, GraphicsAPI* api) : scene(scene), entity(entityID), renderer(api) {}
	~UIComponentProcessor() = default;
	void setEntity(Entity entityID) { entity = entityID; compModifyFunction = nullptr; compAddFunction = nullptr; }
	const char* getPreviewComponentName() const { return previewComponent; }
	void createComponentUI(float deltaTime) { if (compModifyFunction != nullptr) (this->*compModifyFunction)(deltaTime); }
	void addComponent() { if (compAddFunction != nullptr) (this->*compAddFunction)(); }

	template<typename Type> void process();
	template<typename Type> constexpr const char* getComponentName() const { return "Unset"; }
	template<typename Type> void createRemoveComponentButton();

	bool positiveHasComponentCheck = true;

private:
	Scene& scene;
	Entity entity;
	GraphicsAPI* renderer;
	const char* previewComponent = nullptr;
	const char* selectedComponent = nullptr;

	//Rendering component editor
	const char* models[3] = { "Cube", "Sphere", "Plane" };
	const char* previewModel = models[0];
	
	template<typename Type> void _createComponentUI(float);
	template<typename Type> void _addComponent();

	using ComponentUIFunction = void (UIComponentProcessor::*)(float);
	using ComponentAddFunction = void (UIComponentProcessor::*)();
	ComponentAddFunction compAddFunction = nullptr;
	ComponentUIFunction compModifyFunction = nullptr;

	char tagNameBuffer[50] = "";
};

template<> inline constexpr const char* UIComponentProcessor::getComponentName<RenderingComponent>() const { return "Rendering component"; }
template<> inline constexpr const char* UIComponentProcessor::getComponentName<LightComponent>() const { return "Light component"; }
template<> inline constexpr const char* UIComponentProcessor::getComponentName<TextureComponent>() const { return "Texture component"; }
template<> inline constexpr const char* UIComponentProcessor::getComponentName<ScriptingComponent>() const { return "Scripting component"; }
template<> inline constexpr const char* UIComponentProcessor::getComponentName<TagComponent>() const { return "Tag component"; }
template<> inline constexpr const char* UIComponentProcessor::getComponentName<TransformComponent>() const { return "Transform component"; }

template<typename Type>
inline void UIComponentProcessor::process()
{
	const char* componentName = getComponentName<Type>();
	if (!scene.hasComponent<Type>(entity) && !positiveHasComponentCheck)
	{
		bool isSelected = (previewComponent == componentName);
		if (ImGui::Selectable(componentName, isSelected))
		{
			compAddFunction = &UIComponentProcessor::_addComponent<Type>;
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
		if (ImGui::Selectable(componentName, isSelected))
		{
			compModifyFunction = &UIComponentProcessor::_createComponentUI<Type>;
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
inline void UIComponentProcessor::_createComponentUI(float deltaTime)
{
	LOG("Unimplemented");
}

template<typename Type>
inline void UIComponentProcessor::_addComponent()
{
	scene.addComponent<Type>(entity);
	previewComponent = nullptr;
	compAddFunction = nullptr;
	compModifyFunction = &UIComponentProcessor::_createComponentUI<Type>;
}

template<>
inline void UIComponentProcessor::_createComponentUI<RenderingComponent>(float)
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
		Mesh* mesh = renderer->createMesh();
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
inline void UIComponentProcessor::_createComponentUI<TagComponent>(float)
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
inline void UIComponentProcessor::_createComponentUI<TransformComponent>(float deltaTime)
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
	if (ImGui::SliderFloat("Uniform", &scale.x, 0.f, 2.f))
	{
		scale.y = scale.z = scale.x;
		changed = true;
	}
	ImGui::Unindent();
	if (changed)
	{
		component.scale((scale - 1.f) * deltaTime + 1.f);
	}
}
