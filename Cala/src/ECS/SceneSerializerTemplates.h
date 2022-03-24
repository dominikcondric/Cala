#pragma once
#include <filesystem>
#include <sstream>
#include "Graphics/GraphicsAPI.h"
#include <cstring>

using namespace rapidxml;

template<typename Type>
inline void SceneSerializer::process()
{
	serializeComponent<Type>();
}

template<>
inline float SceneSerializer::parseString<float>(const std::string& string) const
{
	return std::stof(string);
}

template<>
inline bool SceneSerializer::parseString<bool>(const std::string& string) const
{
	return bool(std::stoi(string));
}

template<>
inline int SceneSerializer::parseString<int>(const std::string& string) const
{
	return std::stoi(string);
}

template<>
inline double SceneSerializer::parseString<double>(const std::string& string) const
{
	return std::stod(string);
}


template<>
inline long SceneSerializer::parseString<long>(const std::string& string) const
{
	return std::stol(string);
}

template<>
inline void SceneSerializer::serializeComponent<TagComponent>()
{
	xml_node<char>* tagNode = xmlScene.allocate_node(node_element, "TagComponent");
	xmlScene.first_node()->append_node(tagNode);
	for (Entity e : scene->getComponentEntityList<TagComponent>())
	{
		TagComponent& tagComp = scene->getComponent<TagComponent>(e);
		xml_node<char>* attributesNode = xmlScene.allocate_node(node_element, "Entity");
		tagNode->append_node(attributesNode);

		xml_attribute<char>* attribute = xmlScene.allocate_attribute("entityID", xmlScene.allocate_string(std::to_string(e).c_str()));
		attributesNode->append_attribute(attribute);

		attribute = xmlScene.allocate_attribute("name", tagComp.name.c_str());
		attributesNode->append_attribute(attribute);
	}
}

template<>
inline void SceneSerializer::serializeComponent<RenderingComponent>()
{
	xml_node<char>* renderingNode = xmlScene.allocate_node(node_element, "RenderingComponent");
	xmlScene.first_node()->append_node(renderingNode);
	for (Entity e : scene->getComponentEntityList<RenderingComponent>())
	{
		RenderingComponent& renderingComp = scene->getComponent<RenderingComponent>(e);
		xml_node<char>* attributesNode = xmlScene.allocate_node(node_element, "Entity");
		renderingNode->append_node(attributesNode);
		std::string colorString = "";
		colorString += std::to_string(renderingComp.color.x); 
		colorString += " ";
		colorString += std::to_string(renderingComp.color.y); 
		colorString += " ";
		colorString += std::to_string(renderingComp.color.z); 
		colorString += " ";
		colorString += std::to_string(renderingComp.color.w); 

		xml_attribute<char>* attribute = xmlScene.allocate_attribute("entityID", xmlScene.allocate_string(std::to_string(e).c_str()));
		attributesNode->append_attribute(attribute);

		attribute = xmlScene.allocate_attribute("color", xmlScene.allocate_string(colorString.c_str()));
		attributesNode->append_attribute(attribute);

		attribute = xmlScene.allocate_attribute("lightened", xmlScene.allocate_string(std::to_string(renderingComp.lightened).c_str()));
		attributesNode->append_attribute(attribute);

		attribute = xmlScene.allocate_attribute("culled", xmlScene.allocate_string(std::to_string(renderingComp.culled).c_str()));
		attributesNode->append_attribute(attribute);

		attribute = xmlScene.allocate_attribute("wireframe", xmlScene.allocate_string(std::to_string(renderingComp.wireframe).c_str()));
		attributesNode->append_attribute(attribute);

		attribute = xmlScene.allocate_attribute("outlined", xmlScene.allocate_string(std::to_string(renderingComp.outlined).c_str()));
		attributesNode->append_attribute(attribute);

		attribute = xmlScene.allocate_attribute("material", xmlScene.allocate_string(std::to_string((int)renderingComp.material).c_str()));
		attributesNode->append_attribute(attribute);

		attribute = xmlScene.allocate_attribute("mesh", xmlScene.allocate_string(renderingComp.mesh->getMeshFile().c_str()));
		attributesNode->append_attribute(attribute);
	}
}

template<>
inline void SceneSerializer::serializeComponent<LightComponent>()
{
	xml_node<char>* lightNode = xmlScene.allocate_node(node_element, "LightComponent");
	xmlScene.first_node()->append_node(lightNode);
	for (Entity e : scene->getComponentEntityList<LightComponent>())
	{
		LightComponent& lightComponent = scene->getComponent<LightComponent>(e);
		xml_node<char>* attributesNode = xmlScene.allocate_node(node_element, "Entity");
		lightNode->append_node(attributesNode);

		xml_attribute<char>* attribute = xmlScene.allocate_attribute("entityID", xmlScene.allocate_string(std::to_string(e).c_str()));
		attributesNode->append_attribute(attribute);

		attribute = xmlScene.allocate_attribute("lightType", xmlScene.allocate_string(std::to_string((int)lightComponent.type).c_str()));
		attributesNode->append_attribute(attribute);

		attribute = xmlScene.allocate_attribute("coneCutoff", xmlScene.allocate_string(std::to_string(lightComponent.coneCutoff).c_str()));
		attributesNode->append_attribute(attribute);

		attribute = xmlScene.allocate_attribute("intensity", xmlScene.allocate_string(std::to_string(lightComponent.intensity).c_str()));
		attributesNode->append_attribute(attribute);

		std::string colorString = "";
		colorString += std::to_string(lightComponent.color.x);
		colorString += " ";
		colorString += std::to_string(lightComponent.color.y);
		colorString += " ";
		colorString += std::to_string(lightComponent.color.z);

		attribute = xmlScene.allocate_attribute("color", xmlScene.allocate_string(colorString.c_str()));
		attributesNode->append_attribute(attribute);
	}
}

template<>
inline void SceneSerializer::serializeComponent<TransformComponent>()
{
	xml_node<char>* transformNode = xmlScene.allocate_node(node_element, "TransformComponent");
	xmlScene.first_node()->append_node(transformNode);
	for (Entity e : scene->getComponentEntityList<TransformComponent>())
	{
		TransformComponent& transformComponent = scene->getComponent<TransformComponent>(e);
		xml_node<char>* attributesNode = xmlScene.allocate_node(node_element, "Entity");
		transformNode->append_node(attributesNode);

		xml_attribute<char>* attribute = xmlScene.allocate_attribute("entityID", xmlScene.allocate_string(std::to_string(e).c_str()));
		attributesNode->append_attribute(attribute);

		std::string transformString;
		transformString += std::to_string(transformComponent.getTranslation().x) + " ";
		transformString += std::to_string(transformComponent.getTranslation().y) + " ";
		transformString += std::to_string(transformComponent.getTranslation().z);

		attribute = xmlScene.allocate_attribute("translation", xmlScene.allocate_string(transformString.c_str()));
		attributesNode->append_attribute(attribute);

		transformString.clear();
		transformString += std::to_string(transformComponent.getScale().x) + " ";
		transformString += std::to_string(transformComponent.getScale().y) + " ";
		transformString += std::to_string(transformComponent.getScale().z);

		attribute = xmlScene.allocate_attribute("scale", xmlScene.allocate_string(transformString.c_str()));
		attributesNode->append_attribute(attribute);

		transformString.clear();
		transformString += std::to_string(transformComponent.getRotationQuat().x) + " ";
		transformString += std::to_string(transformComponent.getRotationQuat().y) + " ";
		transformString += std::to_string(transformComponent.getRotationQuat().z) + " ";
		transformString += std::to_string(transformComponent.getRotationQuat().w);

		attribute = xmlScene.allocate_attribute("rotation", xmlScene.allocate_string(transformString.c_str()));
		attributesNode->append_attribute(attribute);
	}
}

template<>
inline void SceneSerializer::serializeComponent<TextureComponent>()
{
	xml_node<char>* textureNode = xmlScene.allocate_node(node_element, "TextureComponent");
	xmlScene.first_node()->append_node(textureNode);
	for (Entity e : scene->getComponentEntityList<TextureComponent>())
	{
		TextureComponent& textureComponent = scene->getComponent<TextureComponent>(e);
		xml_node<char>* attributesNode = xmlScene.allocate_node(node_element, "Entity");
		textureNode->append_node(attributesNode);

		xml_attribute<char>* attribute = xmlScene.allocate_attribute("entityID", xmlScene.allocate_string(std::to_string(e).c_str()));
		attributesNode->append_attribute(attribute);

		std::string imagePath = textureComponent.diffuseMap.get() == nullptr ? "None" : textureComponent.diffuseMap->getImagePath();
		attribute = xmlScene.allocate_attribute("diffuseMapPath", xmlScene.allocate_string(imagePath.c_str()));
		attributesNode->append_attribute(attribute);

		imagePath = textureComponent.specularMap.get() == nullptr ? "None" : textureComponent.specularMap->getImagePath();
		attribute = xmlScene.allocate_attribute("specularMapPath", xmlScene.allocate_string(imagePath.c_str()));
		attributesNode->append_attribute(attribute);

		imagePath = textureComponent.normalMap.get() == nullptr ? "None" : textureComponent.normalMap->getImagePath();
		attribute = xmlScene.allocate_attribute("normalMapPath", xmlScene.allocate_string(imagePath.c_str()));
		attributesNode->append_attribute(attribute);

		attribute = xmlScene.allocate_attribute("skyboxReflection", xmlScene.allocate_string(std::to_string(textureComponent.skyboxReflection).c_str()));
		attributesNode->append_attribute(attribute);
	}
}

template<>
inline void SceneSerializer::serializeComponent<SkyboxComponent>()
{
	xml_node<char>* skyboxNode = xmlScene.allocate_node(node_element, "SkyboxComponent");
	xmlScene.first_node()->append_node(skyboxNode);
	for (Entity e : scene->getComponentEntityList<SkyboxComponent>())
	{
		SkyboxComponent& skyboxComponent = scene->getComponent<SkyboxComponent>(e);
		xml_node<char>* attributesNode = xmlScene.allocate_node(node_element, "Entity");
		skyboxNode->append_node(attributesNode);

		xml_attribute<char>* attribute = xmlScene.allocate_attribute("entityID", xmlScene.allocate_string(std::to_string(e).c_str()));
		attributesNode->append_attribute(attribute);

		std::string imagePath = skyboxComponent.texture.get() == nullptr ? "None" : skyboxComponent.texture->getImagePath();
		attribute = xmlScene.allocate_attribute("texturePath", xmlScene.allocate_string(imagePath.c_str()));
		attributesNode->append_attribute(attribute);

		attribute = xmlScene.allocate_attribute("blurValue", xmlScene.allocate_string(std::to_string(skyboxComponent.blurLevel).c_str()));
		attributesNode->append_attribute(attribute);
	}
}

template<>
inline void SceneSerializer::loadComponents<RenderingComponent>(xml_node<char>* entityNode, Entity entityID)
{
	RenderingComponent& component = scene->addAndReturnComponent<RenderingComponent>(entityID);

	xml_attribute<char>* attribute = entityNode->first_attribute();
	do {
		if (!strcmp(attribute->name(), "color"))
		{
			std::istringstream valueString(attribute->value());
			std::string word;
			float* colorPointer = &component.color.x;
			while (!valueString.eof())
			{
				valueString >> word;
				*colorPointer = parseString<float>(word);
				++colorPointer;
			}
		}
		else if (!strcmp(attribute->name(), "lightened"))
		{
			component.lightened = parseString<bool>(attribute->value());
		}
		else if (!strcmp(attribute->name(), "outlined"))
		{
			component.outlined = parseString<bool>(attribute->value());
		}
		else if (!strcmp(attribute->name(), "wireframe"))
		{
			component.wireframe = parseString<bool>(attribute->value());
		}
		else if (!strcmp(attribute->name(), "culled"))
		{
			component.culled = parseString<bool>(attribute->value());
		}
		else if (!strcmp(attribute->name(), "material"))
		{
			component.material = (RenderingComponent::Material)parseString<int>(attribute->value());
		}
		else if (!strcmp(attribute->name(), "mesh"))
		{
			component.mesh.reset(GraphicsAPI::createMesh());
			if (!strcmp(attribute->value(), "Cube"))
				component.mesh->loadCube();
			else if (!strcmp(attribute->value(), "Plane"))
				component.mesh->loadPlane();
			else if (!strcmp(attribute->value(), "Sphere"))
				component.mesh->loadSphere();
			else if (!strcmp(attribute->value(), "Ray"))
				component.mesh->loadRay();
		}
	} while ((attribute = attribute->next_attribute()) != nullptr);
}

#include <glm/gtx/string_cast.hpp>

template<>
inline void SceneSerializer::loadComponents<TransformComponent>(xml_node<char>* entityNode, Entity entityID)
{
	TransformComponent& component = scene->getComponent<TransformComponent>(entityID);
	xml_attribute<char>* attribute = entityNode->first_attribute();
	do {
		if (!strcmp(attribute->name(), "translation"))
		{
			std::istringstream valueString(attribute->value());
			std::string word;
			glm::vec3 translation(0.f);
			float* valuePointer = &translation.x;
			while (!valueString.eof())
			{
				valueString >> word;
				*valuePointer = parseString<float>(word);
				++valuePointer;
			}

			component.translate(translation);
		}
		else if(!strcmp(attribute->name(), "scale"))
		{
			std::istringstream valueString(attribute->value());
			std::string word;
			glm::vec3 scale(0.f);
			float* valuePointer = &scale.x;
			while (!valueString.eof())
			{
				valueString >> word;
				*valuePointer = parseString<float>(word);
				++valuePointer;
			}

			component.scale(scale);
		}
		else if (!strcmp(attribute->name(), "rotation"))
		{
			std::istringstream valueString(attribute->value());
			std::string word;
			glm::vec4 rotation(0.f);
			float* valuePointer = &rotation.x;
			while (!valueString.eof())
			{
				valueString >> word;
				*valuePointer = parseString<float>(word);
				++valuePointer;
			}

			component.rotate(glm::degrees(rotation.a), glm::vec3(rotation.y, rotation.z, rotation.w));
		}
	} while ((attribute = attribute->next_attribute()) != nullptr);
}

template<>
inline void SceneSerializer::loadComponents<LightComponent>(xml_node<char>* entityNode, Entity entityID)
{
	LightComponent& component = scene->addAndReturnComponent<LightComponent>(entityID);
	xml_attribute<char>* attribute = entityNode->first_attribute();
	do {
		if (!strcmp(attribute->name(), "lightType"))
		{
			component.type = (LightComponent::LightSourceType)parseString<int>(attribute->value());
		}
		else if (!strcmp(attribute->name(), "coneCutoff"))
		{
			component.coneCutoff = parseString<float>(attribute->value());
		}
		else if (!strcmp(attribute->name(), "color"))
		{
			std::istringstream valueString(attribute->value());
			std::string word;
			float* valuePointer = &component.color.x;
			while (!valueString.eof())
			{
				valueString >> word;
				*valuePointer = parseString<float>(word);
				++valuePointer;
			}
		}
		else if (!strcmp(attribute->name(), "intensity"))
		{
			component.intensity = parseString<float>(attribute->value());
		}
	} while ((attribute = attribute->next_attribute()) != nullptr);
}

template<>
inline void SceneSerializer::loadComponents<TextureComponent>(xml_node<char>* entityNode, Entity entityID)
{
	TextureComponent& component = scene->addAndReturnComponent<TextureComponent>(entityID);
	xml_attribute<char>* attribute = entityNode->first_attribute();
	Texture::TextureSpecification texSpec;
	do {
		if (!strcmp(attribute->name(), "diffuseMapPath") && strcmp(attribute->value(), "None"))
		{
			component.diffuseMap.reset(GraphicsAPI::createTexture());
			component.diffuseMap->load2DTexture(Image(attribute->value()), texSpec);
		}
		else if (!strcmp(attribute->name(), "specularMapPath") && strcmp(attribute->value(), "None"))
		{
			component.specularMap.reset(GraphicsAPI::createTexture());
			component.specularMap->load2DTexture(Image(attribute->value()), texSpec);
		}
		else if (!strcmp(attribute->name(), "normalMapPath") && strcmp(attribute->value(), "None"))
		{
			component.normalMap.reset(GraphicsAPI::createTexture());
			component.normalMap->load2DTexture(Image(attribute->value()), texSpec);
		}
		else if (!strcmp(attribute->name(), "skyboxReflection"))
		{
			component.skyboxReflection = parseString<bool>(attribute->value());
		}
	} while ((attribute = attribute->next_attribute()) != nullptr);
}

template<>
inline void SceneSerializer::loadComponents<ScriptingComponent>(xml_node<char>* entityNode, Entity entityID)
{
	/*ScriptingComponent& component = scene->addAndReturnComponent<ScriptingComponent>(entityID);
	xml_attribute<char>* attribute = entityNode->first_attribute();
	do {

	} while ((attribute = attribute->next_attribute()) != nullptr);*/
}

template<>
inline void SceneSerializer::loadComponents<SkyboxComponent>(xml_node<char>* entityNode, Entity entityID)
{
	/*SkyboxComponent& component = scene->addAndReturnComponent<SkyboxComponent>(entityID);
	xml_attribute<char>* attribute = entityNode->first_attribute();
	do {

	} while ((attribute = attribute->next_attribute()) != nullptr);*/
}

template<>
inline void SceneSerializer::loadComponents<TagComponent>(xml_node<char>* entityNode, Entity entityID)
{
	TagComponent& component = scene->getComponent<TagComponent>(entityID);
	xml_attribute<char>* attribute = entityNode->first_attribute("name");
	component.name = attribute->value();
}