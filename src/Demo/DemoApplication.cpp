#include "DemoApplication.h"
#include <glm/gtc/random.hpp>

#define LIGHT_MOVE 10.f * time.deltaTime

DemoApplication::DemoApplication() : BaseApplication(Window::Specification("Demo", 1920, 1080, 4)),
sphereMesh(Model().loadSphere(5, 10)), cubeMesh(Model().loadCube()), planeMesh(Model().loadPlane())
{
	camera.setProjectionViewingAngle(90.f);
	camera.setProjectionFarPlane(100.f);

	std::filesystem::path texturesDirectory = std::filesystem::path(__FILE__).parent_path();

	// Reserves are VERY important for memory consistency (pointers!!!)
	transformations.reserve(100);
	renderables.reserve(100);
	for (int i = 0; i < 100; ++i) {
		Transformation transformation;
		glm::vec2 diskRand = glm::diskRand(20.f);
		transformation.scale(glm::linearRand(0.5f, 2.f)).translate(glm::vec3(diskRand.x, glm::linearRand(4.f, 8.f), diskRand.y));
		transformations.push_back(transformation);

		renderables.push_back(LightRenderer::Renderable(
			cubeMesh, transformations.back(), glm::vec4((glm::ballRand(1.f) + 1.f) * 0.5f, 1.f),
			nullptr, nullptr, nullptr, 0.05f, 0.3f, 0.8f, 40.f
		));
	}

	wallTransforms[0].translate(glm::vec3(0.f, 10.f, 20.f)).scale(glm::vec3(40.f, 20.f, 0.5f)); // Up wall
	wallTransforms[3].translate(glm::vec3(0.f, 10.f, -20.f)).scale(glm::vec3(40.f, 20.f, 0.5f)); // Down wall
	wallTransforms[1].translate(glm::vec3(20.f, 10.f, 0.f)).scale(glm::vec3(40.f, 20.f, 0.5f)).rotate(90.f, glm::vec3(0.f, 1.f, 0.f)); // Right wall
	wallTransforms[2].translate(glm::vec3(-20.f, 10.f, 0.f)).scale(glm::vec3(40.f, 20.f, 0.5f)).rotate(90.f, glm::vec3(0.f, 1.f, 0.f)); // Left wall

	planeTransform.scale(40.f);

	glm::vec2 rand = glm::diskRand(10.f);
	lightTransformation.translate(glm::vec3(rand.x, 7.f, rand.y)).scale(0.2f);

	api->setBufferClearingColor(glm::vec4(0.f));
}

void DemoApplication::loop()
{
	api->activateDefaultFramebuffer();
	api->clearFramebuffer();

	lightRenderer.pushLight(
		LightRenderer::Light(
			LightRenderer::Light::Type::Point, lightTransformation, 1.f, glm::vec3(1.f, 1.f, 1.f), 45.f, true
		)
	);

	simpleRenderer.pushRenderable(
		SimpleRenderer::Renderable(
			sphereMesh, lightTransformation, glm::vec4(1.f)
		)
	);

	for (const auto& w : wallTransforms)
	{
		lightRenderer.pushRenderable(
			LightRenderer::Renderable(
				cubeMesh, w, glm::vec4(0.7f, 0.3f, 0.1f, 1.f), nullptr, nullptr,
				nullptr, 0.04f, 0.9f, 0.2f, 5.f
			)
		);
	}

	for (const auto& renderable : renderables)
		lightRenderer.pushRenderable(renderable);

	lightRenderer.pushRenderable(LightRenderer::Renderable(
		planeMesh, planeTransform, glm::vec4(1.f, 1.f, 1.f, 1.f), nullptr, nullptr, nullptr,
		0.03f, 1.f, 0.7f, 200.f
	));

	lightRenderer.render(api.get(), camera);
	simpleRenderer.render(api.get(), camera);

	const IOSystem& io = window->getIO();
	const float moveFactor = 10.f;

	if (io.isKeyPressed(IOSystem::KeyCode::KEY_LEFT)) 
		lightTransformation.translate(glm::vec3(-time.getDeltaTime() * moveFactor, 0.f, 0.f));

	if (io.isKeyPressed(IOSystem::KeyCode::KEY_RIGHT)) 
		lightTransformation.translate(glm::vec3(time.getDeltaTime() * moveFactor, 0.f, 0.f));

	if (io.isKeyPressed(IOSystem::KeyCode::KEY_UP)) 
		lightTransformation.translate(glm::vec3(0.f, 0.f, -time.getDeltaTime() * moveFactor));

	if (io.isKeyPressed(IOSystem::KeyCode::KEY_DOWN)) 
		lightTransformation.translate(glm::vec3(0.f, 0.f, time.getDeltaTime() * moveFactor));
}
