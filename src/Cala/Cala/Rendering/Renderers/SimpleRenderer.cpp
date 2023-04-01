#include "SimpleRenderer.h"

namespace Cala {
	SimpleRenderer::SimpleRenderer()
	{
		std::filesystem::path shadersDir(SHADERS_DIR);
		shader.attachShader(Shader::ShaderType::VertexShader, shadersDir / "GeneralVertexShader.glsl");
		shader.attachShader(Shader::ShaderType::FragmentShader, shadersDir / "LightFragmentShader.glsl");
		shader.createProgram();

		mvpBuffer.setData(shader.getConstantBufferInfo("MVP"), true);
		materialsBuffer.setData(shader.getConstantBufferInfo("MeshData"), true);
		uint32_t lightened = 0;
		materialsBuffer.updateData("lightened", &lightened, sizeof(uint32_t));
	}

	void SimpleRenderer::render(GraphicsAPI* const api, const Camera& camera)
	{
		shader.activate();
		mvpBuffer.updateData("projection", &camera.getProjection()[0][0], sizeof(glm::mat4));
		mvpBuffer.updateData("view", &camera.getView()[0][0], sizeof(glm::mat4));

		int lightened = 0;
		materialsBuffer.updateData("lightened", &lightened, sizeof(int));

		api->enableSetting(GraphicsAPI::FaceCulling);
		api->enableSetting(GraphicsAPI::DepthTesting);
		while (!renderablesStack.empty())
		{
			const auto& renderable = renderablesStack.top();
			materialsBuffer.updateData("material.color", &renderable.color.x, sizeof(glm::vec4));
			mvpBuffer.updateData("model", &renderable.transformation.getTransformMatrix()[0][0], sizeof(glm::mat4));
			api->render(renderable.mesh);
			renderablesStack.pop();
		}

		api->disableSetting(GraphicsAPI::FaceCulling);
		api->disableSetting(GraphicsAPI::DepthTesting);
	}

	void SimpleRenderer::pushRenderable(const Renderable& renderable)
	{
		renderablesStack.push(renderable);
	}
}
