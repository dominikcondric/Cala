#pragma once 
#include "IRenderer.h"
#include "Cala/Rendering/Texture.h"
#include "Cala/Utility/Transformation.h"
#include "Cala/Rendering/Framebuffer.h"

#define MAX_LIGHTS_COUNT 8

namespace Cala {
	class LightRenderer : public IRenderer {
	public:
		LightRenderer();
		~LightRenderer() override = default;
		void render(GraphicsAPI* const api, const Camera& camera) override;

		struct Renderable {
			Renderable(const Mesh& _mesh, const Transformation& _transformation, const glm::vec4& _color,
				const Texture* _diffuseMap, const Texture* _specularMap, const Texture* _normalMap,
				float _ambientCoefficient, float _diffuseCoefficient, float _specularCoefficient, float _shininess) :
				mesh(_mesh), transformation(_transformation), ambientCoefficient(_ambientCoefficient),
				diffuseCoefficient(_diffuseCoefficient), specularCoefficient(_specularCoefficient), color(_color),
				shininess(_shininess), diffuseMap(_diffuseMap), specularMap(_specularMap), normalMap(_normalMap)
			{
			}

			~Renderable() = default;

			const Mesh& mesh;
			const Transformation& transformation;
			glm::vec4 color;
			const Texture* diffuseMap = nullptr;
			const Texture* specularMap = nullptr;
			const Texture* normalMap = nullptr;
			float ambientCoefficient;
			float diffuseCoefficient;
			float specularCoefficient;
			float shininess;
		};

		struct Light {
			enum class Type {
				Point,
				Directional,
				Spotlight
			};

			Light(Type _type, const Transformation& _transformation, float _intensity,
				const glm::vec3& _color, float _spotlightCutoff, bool _shadowCaster) :
				type(_type), transformation(_transformation), color(_color), intensity(_intensity),
				spotlightCutoff(_spotlightCutoff), shadowCaster(_shadowCaster) {}
			Light(const Transformation& _transformation) : transformation(_transformation) {}


			Type type = Type::Point;
			float intensity = 1.f;
			glm::vec3 color{ 1.f };
			const Transformation& transformation;
			float spotlightCutoff = 90.f;
			bool shadowCaster = true;
		};

		void pushRenderable(const Renderable& renderable);
		void pushLight(const Light& light);
        bool shadows = true;

    private:
        void updateLight(const Light &light, uint32_t lightIndex, GraphicsAPI *const api);
		std::vector<Renderable> renderables[8];
		std::vector<Light> lights;
		Shader mainShader;
		Shader shadowPassShader;
		ConstantBuffer mvpBuffer;
		ConstantBuffer materialsBuffer;
		ConstantBuffer lightsBuffer;
		Framebuffer shadowsFramebuffer;
	};
}