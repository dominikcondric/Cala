#include <Utility/Model.h>
#include <Rendering/Texture.h>
#include <Rendering/Renderers/LightRenderer.h>
#include <Rendering/Renderers/SimpleRenderer.h>
#include "Utility/Window.h"
#include <glm/gtc/random.hpp>

#define MOVE_SPEED 5.f * deltaTime

using namespace Cala;

class Application {
public:
	Application();
	~Application();
	void run();

private:
	void update();

	Unique<GraphicsAPI> api;
	Window window;
	PerspectiveCamera camera;
};

Application::Application() : window({ "Demo", 1920, 1080, 4 })
{
}

Application::~Application()
{
	window.shutdown();
}

void Application::update()
{
	const IOSystem& ioSystem = window.getIO();
	const float deltaTime = window.getTime().deltaTime;

	if (ioSystem.isMouseButtonPressed(IOSystem::MOUSE_BUTTON_RIGHT))
		camera.rotateCamera(ioSystem.getCursorOffset());

	if (ioSystem.isKeyPressed(IOSystem::KEY_W))
		camera.moveCamera(Camera::Directions::FORWARD, MOVE_SPEED);

	if (ioSystem.isKeyPressed(IOSystem::KEY_S))
		camera.moveCamera(Camera::Directions::BACKWARD, MOVE_SPEED);

	if (ioSystem.isKeyPressed(IOSystem::KEY_A))
		camera.moveCamera(Camera::Directions::LEFT, MOVE_SPEED);

	if (ioSystem.isKeyPressed(IOSystem::KEY_D))
		camera.moveCamera(Camera::Directions::RIGHT, MOVE_SPEED);

	if (ioSystem.isKeyPressed(IOSystem::KEY_SPACE))
		camera.moveCamera(Camera::Directions::UP, MOVE_SPEED);

	if (ioSystem.isKeyPressed(IOSystem::KEY_LEFT_SHIFT))
		camera.moveCamera(Camera::Directions::DOWN, MOVE_SPEED);

	window.update();
	if (window.isResized())
	{
		auto winSize = window.getWindowSize();
		api->setViewport(glm::uvec4(0, 0, winSize.x, winSize.y));
		camera.setProjectionAspectRatio((float)winSize.x / winSize.y);
	}
}

void Application::run()
{
	window.createOpenGLContext();
	
	std::filesystem::path texturesDirectory = std::filesystem::path(__FILE__).parent_path() / "textures";
	
	LightRenderer lightRenderer;
	auto light = LightRenderer::Light(LightRenderer::Light::Type::Point, 
		Transformation().translate(glm::vec3(1.f, 5.f, 0.f)), 1.f, glm::vec3(1.f), 0.f);
	
	Mesh sphereMesh(Model().loadSphere());
	SimpleRenderer::Renderable lightRenderable(sphereMesh, light.transformation.scale(0.2f), glm::vec4(1.f));
	SimpleRenderer simpleRenderer;

	std::vector<Texture> diffuseMaps;
	std::vector<Texture> normalMaps;
	Texture::Specification texSpec;
	for (int i = 0; i < 20; ++i)
	{
		int randomTextureIndex = glm::linearRand(201, 250);
		diffuseMaps.push_back(Texture(Image(texturesDirectory / (std::to_string(randomTextureIndex) + ".jpg")), texSpec));
		normalMaps.push_back(Texture(Image(texturesDirectory / (std::to_string(randomTextureIndex) + "_norm.jpg")), texSpec));
	}

	std::vector<LightRenderer::Renderable> renderables;
	for (int i = 0; i < 200; ++i) {
		int textureIndex = glm::linearRand(0, (int)diffuseMaps.size() - 1);
		renderables.push_back(LightRenderer::Renderable(
			sphereMesh, Transformation().scale(glm::linearRand(0.2f, 3.f)).translate(glm::ballRand(30.f)), glm::vec4(1.f), 
			&diffuseMaps[textureIndex], nullptr, &normalMaps[textureIndex], 0.2f, 0.9f, 0.8f, 10.f
		));
	}

	camera.setPosition(glm::vec3(0.f, 5.f, 5.f));
	camera.setProjectionAspectRatio((float)window.getWindowSize().x / window.getWindowSize().y);
	camera.setProjectionFarPlane(200.f);

	api.reset(GraphicsAPI::construct());
	api->setBufferClearingColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.f));
	api->setBufferClearingBits(true, true, false);
	api->setViewport(glm::ivec4(0, 0, window.getWindowSize().x, window.getWindowSize().y));

	while (!window.exitTriggered()) {
		api->clearFramebuffer();
		
		lightRenderer.pushLight(light);
		for (const auto& renderable : renderables)
			lightRenderer.pushRenderable(renderable);
		lightRenderer.render(api.get(), camera);

		simpleRenderer.pushRenderable(lightRenderable);
		simpleRenderer.render(api.get(), camera);


		update();
	}
}

int main(void)
{
	Application* app = new Application;
	app->run();
	delete app;
}
