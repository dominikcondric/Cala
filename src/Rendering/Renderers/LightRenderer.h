#pragma once 
#include "IRenderer.h"
#include "Rendering/Texture.h"
#include "Utility/Transformation.h"
#include <stack>

class LightRenderer : public IRenderer {
public:
	LightRenderer();
	~LightRenderer() override = default;
	void render(const GraphicsAPI* api, const Camera& camera) override;

	struct Renderable {
		Renderable(const Mesh& _mesh, const Transformation& _transformation, float _ambientCoefficient, 
			float _diffuseCoefficient, float _specularCoefficient, float _shininess) :
			mesh(_mesh), transformation(_transformation), ambientCoefficient(_ambientCoefficient), diffuseCoefficient(_diffuseCoefficient),
			specularCoefficient(_specularCoefficient), shininess(_shininess) 
		{
		}

		virtual ~Renderable() = 0;

		const Mesh& mesh;
		Transformation transformation;
		float ambientCoefficient;
		float diffuseCoefficient;
		float specularCoefficient;
		float shininess;
	};

	struct TexturedRenderable : public Renderable {
		TexturedRenderable(const Mesh& _mesh, const Transformation& _transformation, const Texture& _diffuseMap,
			float _ambientCoefficient, float _diffuseCoefficient, float _specularCoefficient, float _shininess) :
			Renderable(_mesh, _transformation, _ambientCoefficient, _diffuseCoefficient, _specularCoefficient,
			_shininess), diffuseMap(_diffuseMap) 
		{
		}

		const Texture& diffuseMap;
	};

	struct TexturedSpecularRenderable : public TexturedRenderable {
		TexturedSpecularRenderable(const Mesh& _mesh, const Transformation& _transformation, const Texture& _diffuseMap,
			const Texture& _specularMap, float _ambientCoefficient, float _diffuseCoefficient, 
			float _specularCoefficient, float _shininess) :
			TexturedRenderable(_mesh, _transformation, _diffuseMap, _ambientCoefficient, _diffuseCoefficient, _specularCoefficient,
				_shininess), specularMap(_specularMap)
		{
		}

		const Texture& specularMap;
	};

	/*struct TexturedSpecularNormalRenderable : public TexturedSpecularRenderable {
		TexturedSpecularNormalRenderable(const Mesh& _mesh, const Transformation& _transformation, const Texture& _diffuseMap,
			const Texture& _specularMap, const Texture& _normalMap, float _ambientCoefficient, float _diffuseCoefficient,
			float _specularCoefficient, float _shininess) :
			TexturedSpecularRenderable(_mesh, _transformation, _diffuseMap, _specularMap, _ambientCoefficient, 
				_diffuseCoefficient, _specularCoefficient, _shininess), normalMap(_normalMap)
		{
		}

		const Texture& normalMap;
	};*/

	struct ColoredRenderable : public Renderable {
		ColoredRenderable(const Mesh& _mesh, const Transformation& _transformation, const glm::vec4& _color,
			float _ambientCoefficient, float _diffuseCoefficient, float _specularCoefficient, float _shininess) :
			Renderable(_mesh, _transformation, _ambientCoefficient, _diffuseCoefficient, 
				_specularCoefficient, _shininess), color(_color) 
		{
		}

		glm::vec4 color;
	};

	struct Light {
		enum class Type {
			Point,
			Directional,
			Spotlight
		};

		Light(Type _type, const Transformation& _transformation, float _intensity, 
			const glm::vec3& _color, float _spotlightCutoff) : 
			type(_type), transformation(_transformation), color(_color), intensity(_intensity),
			spotlightCutoff(_spotlightCutoff) {}

		Type type;
		float intensity;
		glm::vec3 color;
		Transformation transformation;
		float spotlightCutoff;
	};

	void pushTexturedRenderable(const TexturedRenderable& renderable);
	void pushTexturedSpecularRenderable(const TexturedSpecularRenderable& renderable);
	void pushColoredRenderable(const ColoredRenderable& renderable);
	void pushLight(const Light& light);


private:
	void updateLight(const Light& light, uint32_t lightIndex) const;
	std::stack<const ColoredRenderable*> coloredRenderables;
	std::stack<const TexturedRenderable*> texturedRenderables;
	std::stack<const TexturedSpecularRenderable*> texturedSpecularRenderables;
	//std::stack<const TexturedSpecularNormalRenderable*> texturedNormalRenderables;
	std::stack<const Light*> lightsStack;
	Shader shader;
	ConstantBuffer mvpBuffer;
	ConstantBuffer materialsBuffer;
	ConstantBuffer lightsBuffer;
};