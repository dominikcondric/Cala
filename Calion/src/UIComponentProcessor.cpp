#include "UIComponentProcessor.h"

UIComponentProcessor::UIComponentProcessor(Scene& scene, Entity entityID, const Window& window, const GraphicsAPI* api) 
	: scene(scene), entity(entityID),  texturesFileDialog(window), api(api)
{
	texturesFileDialog.setInitialDirectory((Cala::getProjectPath() / "Calion/Textures/").string());
}

void UIComponentProcessor::setEntity(Entity entityID)
{
	entity = entityID;
	compAddFunction = nullptr;
	compModifyFunction = nullptr;
	selectedComponent = nullptr;
	previewComponent = nullptr;
	positiveHasComponentCheck = false;
	processAllComponents(*this);
}

void UIComponentProcessor::addSelectedComponent()
{
	if (compAddFunction != nullptr) 
		(this->*compAddFunction)();

	positiveHasComponentCheck = false;
	processAllComponents(*this);
}

void UIComponentProcessor::createSelectedComponentUI(float deltaTime)
{
	if (compModifyFunction != nullptr) 
		(this->*compModifyFunction)(deltaTime);
}
