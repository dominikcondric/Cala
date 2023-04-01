#include "HelperGridRenderer.h"

namespace Cala {
	HelperGridRenderer::HelperGridRenderer()
	{
		std::filesystem::path shadersDir(SHADERS_DIR);
		shader.attachShader(Shader::ShaderType::VertexShader, shadersDir / "HelperGridVertexShader.glsl");
		shader.attachShader(Shader::ShaderType::GeometryShader, shadersDir / "HelperGridGeometryShader.glsl");
		shader.attachShader(Shader::ShaderType::FragmentShader, shadersDir / "HelperGridFragmentShader.glsl");
		shader.createProgram();

		mvpBuffer.setData(shader.getConstantBufferInfo("MVP"), true);

		gridMesh.loadMesh(Model().loadRay(glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 0.f, -1.f), 2.f), false);

		transformation.scale(glm::vec3((float)gridSize));
	}

	void HelperGridRenderer::render(GraphicsAPI* const api, const Camera& camera)
	{
		shader.activate();
		mvpBuffer.updateData("projection", &camera.getProjection()[0][0], sizeof(glm::mat4));
		mvpBuffer.updateData("view", &camera.getView()[0][0], sizeof(glm::mat4));
		api->enableSetting(GraphicsAPI::Multisampling);
		api->disableSetting(GraphicsAPI::FaceCulling);
		const float distance = glm::floor(glm::length(camera.getPosition()));
		if (distance != updateDistance)
		{
			updateDistance = distance;
			transformation.translate(glm::vec3(glm::floor(camera.getPosition().x), 0.f, glm::floor(camera.getPosition().z)) - transformation.getTranslation());
		}

		mvpBuffer.updateData("model", &transformation.getTransformMatrix()[0][0], sizeof(glm::mat4));
		api->render(gridMesh);
	}
}

