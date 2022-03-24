#pragma once
#include <string>
#include <vector>
#include "Core/Core.h"
#include "Graphics/Mesh.h"
#include "Graphics/Texture.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/vec3.hpp>
#include <set>

class Scene;
namespace Cala { struct Time; }
class IOSystem;

using ComponentID = int;
using ComponentIndex = int;
using Entity = int;

template<typename T>
struct ComponentEntity {
	ComponentEntity(T&& comp, Entity ent) : component(std::move(comp)) { entities.push_back(ent); }
	~ComponentEntity() = default;
	T component;
	std::vector<Entity> entities;
};

struct Component {
	Component() = default;
	virtual ~Component() = default;
};

struct TagComponent : public Component {
	TagComponent(const std::string& tagName = "") : name(tagName) {}
	~TagComponent() override = default;
	std::string name;
};

struct RenderingComponent : public Component {
public:
	RenderingComponent() = default;
	~RenderingComponent() override = default;

	enum class Material {
		PLASTIC,
		METAL,
		FABRIC
	};
	
	Shared<Mesh> mesh;
	glm::vec4 color{ 0.7f, 0.4f, 0.2f, 1.f };
	bool culled{ true };
	bool outlined{ false };
	bool wireframe{ false };
	bool lightened{ true };
	Material material;
};

struct LightComponent : public Component {
	LightComponent() = default;
	~LightComponent() override = default;

	enum LightSourceType {
		Point,
		Directional,
		Cone
	};

	LightSourceType type;
	float intensity{ 1.f };
	float coneCutoff{ glm::radians(12.5f) };
	glm::vec3 color{ 1.f };
};

struct ScriptingComponent : public Component {
public:
	class Script {
	public:
		Script() = default;
		virtual ~Script() = default;
		virtual void onAttach(Scene& scene, Entity entID) = 0;
		virtual void onDestroy(Scene& scene, Entity entID) = 0;
		virtual void onUpdate(Scene& scene, Entity entID, const Cala::Time& time, const IOSystem& io) = 0;
	};

	ScriptingComponent(Script* script) : scriptPointer(script) {}
	ScriptingComponent() = default;
	~ScriptingComponent() = default;
	void attachScript(Script* script) { scriptPointer.reset(script); }
	const Shared<Script>& getScript() const { return scriptPointer; }

private:
	Shared<Script> scriptPointer;
};

struct TransformComponent : public Component {
public:
	TransformComponent() = default;
	~TransformComponent() override = default;
	void translate(const glm::vec3& t);
	void rotate(float angle, const glm::vec3& rotationAxis);
	void scale(const glm::vec3& s);
	const glm::mat4& getTransformMatrix() const { return transformMatrix; }
	const glm::vec3& getTranslation() const { return translationVector; }
	const glm::vec3& getScale() const { return scaleVector; }
	const glm::quat& getRotationQuat() const { return rotationQuat; }
	const glm::mat4 getRotationMatrix() const { return glm::mat4_cast(rotationQuat); }

private:
	glm::mat4 transformMatrix{ 1.f };
	glm::quat rotationQuat{ glm::angleAxis(0.f, glm::vec3(1.f, 0.f, 0.f)) };
	glm::vec3 translationVector{ 0.f, 0.f, 0.f };
	glm::vec3 scaleVector{ 1.f, 1.f, 1.f };
};

struct TextureComponent : public Component {
	TextureComponent() = default;
	~TextureComponent() override = default;
	TextureComponent(const TextureComponent& other) = default;
	TextureComponent(TextureComponent&& other) noexcept = default;
	TextureComponent& operator=(const TextureComponent& other) = default;
	TextureComponent& operator=(TextureComponent&& other) noexcept = default;

	Shared<Texture> diffuseMap;
	Shared<Texture> specularMap;
	Shared<Texture> normalMap;
	bool skyboxReflection = false;
};

struct SkyboxComponent : public Component {
	SkyboxComponent() = default;
	~SkyboxComponent() override = default;
	int blurLevel = 200;
	Shared<Texture> texture;
};