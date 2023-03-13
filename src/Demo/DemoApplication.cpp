#include "DemoApplication.h"

#define LIGHT_MOVE 10.f * time.deltaTime

DemoApplication::DemoApplication() : BaseApplication(Window::Specification("Demo", 1920, 1080, 4)),
light(LightRenderer::Light::Type::Point, Transformation().translate(glm::vec3(1.f, 5.f, 0.f)), 1.f, glm::vec3(1.f), 0.f),
sphereMesh(Model().loadCube()),
lightRenderable(sphereMesh, light.transformation.scale(0.2f), glm::vec4(1.f))
{
	std::filesystem::path texturesDirectory = std::filesystem::path(__FILE__).parent_path() / "textures";

	Texture::Specification texSpec;
	for (int i = 0; i < 20; ++i)
	{
		int randomTextureIndex = glm::linearRand(201, 250);
		diffuseMaps.push_back(Texture(Image(texturesDirectory / (std::to_string(randomTextureIndex) + ".jpg")), texSpec));
		normalMaps.push_back(Texture(Image(texturesDirectory / (std::to_string(randomTextureIndex) + "_norm.jpg")), texSpec));
	}

	for (int i = 0; i < 200; ++i) {
		int textureIndex = glm::linearRand(0, (int)diffuseMaps.size() - 1);
		renderables.push_back(LightRenderer::Renderable(
			sphereMesh, Transformation().scale(glm::linearRand(0.2f, 3.f)).translate(glm::ballRand(30.f)), glm::vec4(1.f),
			&diffuseMaps[textureIndex], nullptr, &normalMaps[textureIndex], 0.2f, 0.9f, 0.8f, 10.f
		));
	}
}

void DemoApplication::loop()
{
	api->clearFramebuffer();

	lightRenderer.pushLight(light);
	for (const auto& renderable : renderables)
		lightRenderer.pushRenderable(renderable);
	lightRenderer.render(api.get(), camera);

	simpleRenderer.pushRenderable(lightRenderable);
	simpleRenderer.render(api.get(), camera);

	const IOSystem& io = window->getIO();

	if (io.isKeyPressed(IOSystem::KEY_UP))
	{
		light.transformation.translate(glm::vec3(0.f, 0.f, -LIGHT_MOVE));
		lightRenderable.transformation.translate(glm::vec3(0.f, 0.f, -LIGHT_MOVE));
	}
	
	if (io.isKeyPressed(IOSystem::KEY_DOWN))
	{
		light.transformation.translate(glm::vec3(0.f, 0.f, LIGHT_MOVE));
		lightRenderable.transformation.translate(glm::vec3(0.f, 0.f, LIGHT_MOVE));
	}

	if (io.isKeyPressed(IOSystem::KEY_LEFT))
	{
		light.transformation.translate(glm::vec3(-LIGHT_MOVE, 0.f, 0.f));
		lightRenderable.transformation.translate(glm::vec3(-LIGHT_MOVE, 0.f, 0.f));
	}

	if (io.isKeyPressed(IOSystem::KEY_RIGHT))
	{
		light.transformation.translate(glm::vec3(LIGHT_MOVE, 0.f, 0.f));
		lightRenderable.transformation.translate(glm::vec3(LIGHT_MOVE, 0.f, 0.f));
	}

	if (io.isKeyPressed(IOSystem::KEY_RIGHT_SHIFT))
	{
		light.transformation.translate(glm::vec3(0.f, LIGHT_MOVE, 0.f));
		lightRenderable.transformation.translate(glm::vec3(0.f, LIGHT_MOVE, 0.f));
	}

	if (io.isKeyPressed(IOSystem::KEY_RIGHT_CONTROL))
	{
		light.transformation.translate(glm::vec3(0.f, -LIGHT_MOVE, 0.f));
		lightRenderable.transformation.translate(glm::vec3(0.f, -LIGHT_MOVE, 0.f));
	}
}
