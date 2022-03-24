#include "SceneSerializer.h"
#include "ComponentIterator.h"
#include <fstream>
#include <sstream>

namespace rapidxml::internal {
	template<class OutIt, class Ch>
	inline OutIt print_children(OutIt out, const xml_node<Ch>* node, int flags, int indent);

	template<class OutIt, class Ch>
	inline OutIt print_attributes(OutIt out, const xml_node<Ch>* node, int flags);

	template<class OutIt, class Ch>
	inline OutIt print_data_node(OutIt out, const xml_node<Ch>* node, int flags, int indent);

	template<class OutIt, class Ch>
	inline OutIt print_cdata_node(OutIt out, const xml_node<Ch>* node, int flags, int indent);

	template<class OutIt, class Ch>
	inline OutIt print_element_node(OutIt out, const xml_node<Ch>* node, int flags, int indent);

	template<class OutIt, class Ch>
	inline OutIt print_declaration_node(OutIt out, const xml_node<Ch>* node, int flags, int indent);

	template<class OutIt, class Ch>
	inline OutIt print_comment_node(OutIt out, const xml_node<Ch>* node, int flags, int indent);

	template<class OutIt, class Ch>
	inline OutIt print_doctype_node(OutIt out, const xml_node<Ch>* node, int flags, int indent);

	template<class OutIt, class Ch>
	inline OutIt print_pi_node(OutIt out, const xml_node<Ch>* node, int flags, int indent);
}

#include <rapidxml_print.hpp>

SceneSerializer::SceneSerializer()
{
	componentLoaderFunctions.insert({ "TagComponent", &SceneSerializer::loadComponents<TagComponent> });
	componentLoaderFunctions.insert({ "RenderingComponent", &SceneSerializer::loadComponents<RenderingComponent> });
	componentLoaderFunctions.insert({ "TransformComponent", &SceneSerializer::loadComponents<TransformComponent> });
	componentLoaderFunctions.insert({ "LightComponent", &SceneSerializer::loadComponents<LightComponent> });
	componentLoaderFunctions.insert({ "TextureComponent", &SceneSerializer::loadComponents<TextureComponent> });
	componentLoaderFunctions.insert({ "SkyboxComponent", &SceneSerializer::loadComponents<SkyboxComponent> });
	componentLoaderFunctions.insert({ "ScriptingComponent", &SceneSerializer::loadComponents<ScriptingComponent> });
}

void SceneSerializer::serializeScene(Scene* scene, const std::filesystem::path& pathToScene)
{
	if (pathToScene.extension() == ".csn" || !pathToScene.has_extension())
	{
		this->scene = scene;
		
		using namespace rapidxml;
		xml_node<char>* rootNode = xmlScene.allocate_node(node_element, "Scene");
		xmlScene.append_node(rootNode);

		processAllComponents(*this);
		std::ofstream file;
		if (pathToScene.has_extension())
			file.open(pathToScene);
		else 
			file.open(pathToScene.string() + ".csn");

		file << xmlScene;
		file.close();
		xmlScene.clear();
	}
}

void SceneSerializer::loadScene(Scene* sceneToLoad, const std::filesystem::path& pathToScene)
{
	if (std::filesystem::exists(pathToScene) && pathToScene.extension() == ".csn")
	{
		scene = sceneToLoad;
		using namespace rapidxml;
		std::string fileData = getFileData(pathToScene);
		xmlScene.parse<0>(fileData.data());
		scene->clearEntities();
		entityCounter = -1;
		xml_node<char>* node = xmlScene.first_node()->first_node();
		do {
			xml_node<char>* entityNode = node->first_node();
			do {
				if (entityNode == nullptr)
					break;

				xml_attribute<char>* entityIDAttribute = entityNode->first_attribute("entityID");
				Entity entityID = parseString<Entity>(entityIDAttribute->value());
				if (entityID > entityCounter)
				{
					scene->addEntity();
					++entityCounter;
				}

				(this->*componentLoaderFunctions.at(node->name()))(entityNode, entityID);
			} while ((entityNode = entityNode->next_sibling()) != nullptr);
		} while ((node = node->next_sibling()) != 0);
	}

	xmlScene.clear();
}

std::string SceneSerializer::getFileData(const std::filesystem::path& pathToScene) const
{
	std::string fileData;
	std::ifstream file;

	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {
		file.open(pathToScene);
		std::stringstream fileStream;
		fileStream << file.rdbuf();
		fileData = fileStream.str();
		file.close();
	}
	catch (std::ifstream::failure& e) {
		std::cout << e.what() << std::endl;
		std::cout << "Error: Cannot find shader file(" << pathToScene.string() << ")!" << std::endl;
	}
	
	return fileData;
}
