#pragma once
#include <stack>
#include <memory>
#include "Camera.h"
#include "Core/Core.h"
#include "GraphicsAPI.h"
#include <bitset>
#include "ECS/Scene.h" 

class RenderingSystem {
public:
	RenderingSystem() = default;
	~RenderingSystem();
	void initialize(GraphicsAPI* api);
	void pushToDrawQueue(Entity drawableEntity, const RenderingComponent& renderingComp);
	void setSkybox(Entity skyboxEnt);
	void draw(const Scene& scene, const Camera& cam);

	bool helperGridEnabled = true;

	enum PostProcessingEffect {
		Bloom			= 0,
		HDR				= 1,
		BoxBlur			= 2,
		EdgeDetection	= 3,
		GaussianBlur	= 4,
		Negative		= 5
	};

	void setPostProcessingEffectState(PostProcessingEffect effect, bool value);
	bool getPostProcessingEffectState(PostProcessingEffect effect) const;
	float& getPostProcessingEffectValue(PostProcessingEffect effect);

private:
	enum HelperPostProcessingEffect {
		HorizontalGaussianBlur		= 6,
		VerticalGaussianBlur		= 7,
		Extract						= 8,
		Copy						= 9
	};

	struct HelperGrid {
		Unique<Mesh> mesh;
		Unique<Shader> shader;
		float updateDistance = 1.f;
		uint32_t gridSize = 50.f;
		glm::mat4 gridTransform{ 1.f };
	};

	struct PostProcessingUtility {
		Unique<Framebuffer> helperFramebuffers[3];
		Unique<Mesh> renderingQuad;
		uint8_t currentFB = 0;
		Unique<Shader> shader;
		Unique<ConstantBuffer> effectsBuffer;
		std::bitset<6> effectsStates;
		float effectsValues[6]{ 0.f };
	};

	Unique<ConstantBuffer> cameraTransforms;

	// Skybox
	Unique<Shader> skyboxShader;
	Unique<Mesh> skyboxMesh;
	Unique<ConstantBuffer> skyboxBlurBuffer;

	Unique<ConstantBuffer> materialsBuffer;
	Unique<ConstantBuffer> lightsBuffer;
	Unique<Shader> generalShader;
	
	PostProcessingUtility postProcessingUtility;
	HelperGrid helperGrid;
	GraphicsAPI* renderer;
	bool initialized = false;

	// Drawing data
	std::stack<Entity> lightenedModelsStack;
	std::stack<Entity> coloredModelsStack;
	Entity skyboxEntity = -1;
	std::stack<std::pair<const RenderingComponent&, const glm::mat4&>> outliningStack;

	// Methods
	void drawColored(const RenderingComponent& renderingComp, const TransformComponent& transformComp) const;
	void drawLightened(const RenderingComponent& renderingComp, const TransformComponent& transformComp) const;
	void drawLightenedTextured(const RenderingComponent& renderingComp, const TransformComponent& transformComp, const TextureComponent& textureComp) const;
	void drawSkybox(const SkyboxComponent& skyboxComp) const;
	void drawHelperGrid(const glm::vec3& camPosition);
	void applyPostProcessingEffect(PostProcessingEffect effect, float effectValue);
	void outlineMeshes();
	void updateShaderLight(const LightComponent& light, const TransformComponent& transform, uint32_t lightIndex) const;
	void updateShaderMaterialData(const glm::vec4& color, RenderingComponent::Material material) const;
	void setupRenderer(const RenderingComponent& renderingComp) const;
	void applyBloom(int bloomIntensity);
	void renderWithPostProcessing(const Scene& scene, const Camera& cam);
	void render(const Scene& scene, const Camera& cam);
};