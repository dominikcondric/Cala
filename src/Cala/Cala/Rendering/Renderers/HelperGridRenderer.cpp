#include "HelperGridRenderer.h"

namespace Cala {
	HelperGridRenderer::HelperGridRenderer()
	{
		std::filesystem::path shadersDir(SHADERS_DIR);
		shader.attachShader(Shader::ShaderType::VertexShader, shadersDir / "HelperGridVertexShader.glsl");
		shader.attachShader(Shader::ShaderType::GeometryShader, shadersDir / "HelperGridGeometryShader.glsl");
		shader.attachShader(Shader::ShaderType::FragmentShader, shadersDir / "LightFragmentShader.glsl");
		shader.createProgram();

		mvpBuffer.setData(shader.getConstantBufferInfo("MVP"), true);
		meshDataBuffer.setData(shader.getConstantBufferInfo("MeshData"), true);

		gridMesh.loadFromModel(Model().loadRay(glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 0.f, -1.f), 2.f), false, false);

		transformation.scale(glm::vec3((float)gridSize));
	}

    void HelperGridRenderer::setupCamera(const Camera &camera)
    {
		mvpBuffer.updateData("projection", &camera.getProjection()[0][0], sizeof(glm::mat4));
		mvpBuffer.updateData("view", &camera.getView()[0][0], sizeof(glm::mat4));

		if (moveXZWithCamera)
		{
			transformation.translate(glm::vec3(glm::floor(camera.getPosition().x), 0.f, glm::floor(camera.getPosition().z)) - transformation.getTranslation());
		}
    }

    void HelperGridRenderer::setMoveXZWithCamera(bool move)
    {
		moveXZWithCamera = move;
		if (!move)
		{
			transformation.translate(-transformation.getTranslation());
		}
    }

    void HelperGridRenderer::render(GraphicsAPI* const api, const Framebuffer* renderingTarget)
	{
		api->activateFramebuffer(renderingTarget);
		shader.activate();
		api->enableSetting(GraphicsAPI::DepthTesting);
		const int lightened = 0;
		meshDataBuffer.updateData("lightened", &lightened, sizeof(int));
		meshDataBuffer.updateData("material.color", &gridColor, sizeof(glm::vec4));
		mvpBuffer.updateData("model", &transformation.getTransformMatrix()[0][0], sizeof(glm::mat4));
		api->render(gridMesh);
		api->disableSetting(GraphicsAPI::DepthTesting);
	}
}

