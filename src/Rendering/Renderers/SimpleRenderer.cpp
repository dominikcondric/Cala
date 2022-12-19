#include "SimpleRenderer.h"

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

void SimpleRenderer::render(const GraphicsAPI* api, const Camera& camera)
{
	shader.activate();
	if (camera.projectionChanged)
	{
		mvpBuffer.updateData("projection", &camera.getProjection()[0][0], sizeof(glm::mat4));
	}

	if (camera.viewChanged)
	{
		mvpBuffer.updateData("view", &camera.getView()[0][0], sizeof(glm::mat4));
	}

	int lightened = 0;
	materialsBuffer.updateData("lightened", &lightened, sizeof(int));

	while (!renderablesStack.empty())
	{
		const auto& renderable = *renderablesStack.top();
		materialsBuffer.updateData("material.color", &renderable.color.x, sizeof(glm::vec4));
		mvpBuffer.updateData("model", &renderable.transformation.getTransformMatrix()[0][0], sizeof(glm::mat4));
		api->render(renderable.mesh);
		renderablesStack.pop();
	}
}

void SimpleRenderer::pushRenderable(const Renderable& renderable)
{
	renderablesStack.push(&renderable);
}
