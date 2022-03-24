#pragma once
#include <filesystem>
#include <rapidxml.hpp>
#include "Scene.h"
#include "ECS/ComponentIterator.h"
#include "Core/Utility/FileDialog.h"

class SceneSerializer : public ComponentProcessor {
public:
	SceneSerializer();
	~SceneSerializer() = default;
	void serializeScene(Scene* scene, const std::filesystem::path& pathToScene);
	void loadScene(Scene* sceneToLoad, const std::filesystem::path& pathToScene);
	template<typename Type> void process();

private:
	template<class T> void serializeComponent() { LOG("Unimplemented!"); }
	template<class T> void loadComponents(rapidxml::xml_node<char>* entityNode, Entity entityID) { LOG("Unimplemented"); }
	template<typename T> T parseString(const std::string& string) const { LOG("Unimplemented!"); }

	std::string getFileData(const std::filesystem::path& pathToScene) const;

	using LoadComponentFunction = void (SceneSerializer::*)(rapidxml::xml_node<char>*, Entity);
	std::unordered_map<std::string, LoadComponentFunction> componentLoaderFunctions;

	rapidxml::xml_document<char> xmlScene;
	Scene* scene = nullptr;
	Entity entityCounter = -1;
};

#include "SceneSerializerTemplates.h"